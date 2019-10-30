/*!
 * \file telecommand_widget.cpp
 * \brief Implementation of a widget for sending commands to the receiver
 * through telnet.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 * -----------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *      Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <https://www.gnu.org/licenses/>.
 *
 * -----------------------------------------------------------------------
 */


#include "telecommand_widget.h"
#include "ui_telecommand_widget.h"
#include <QScrollBar>

TelecommandWidget::TelecommandWidget(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::TelecommandWidget)
{
    ui->setupUi(this);

    ui->autoScrollCheckBox->setChecked(true);
    ui->reconnectCheckBox->setChecked(true);
    ui->clearCheckBox->setChecked(true);

    ui->connectPushButton->setEnabled(false);
    ui->plainTextEdit->setPlaceholderText("The sent and received messages will be logged here...");
    ui->dateTimeEdit->setDate(QDate::currentDate());

    // Disable the command buttons.
    ui->statusPushButton->setEnabled(false);
    ui->standbyPushButton->setEnabled(false);
    ui->resetPushButton->setEnabled(false);
    ui->coldStartPushButton->setEnabled(false);
    ui->warmStartPushButton->setEnabled(false);
    ui->hotStartPushButton->setEnabled(false);

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &TelecommandWidget::reconnect);

    connect(ui->addressLineEdit, &QLineEdit::textChanged, this, &TelecommandWidget::onAddressEditTextchanged);
    connect(ui->portLineEdit, &QLineEdit::textChanged, this, &TelecommandWidget::onPortEditTextchanged);
    connect(this, &TelecommandWidget::inputsChanged, this, &TelecommandWidget::handleInputsChanged);
    connect(ui->clearPushButton, &QPushButton::clicked, this, &TelecommandWidget::clear);

    connect(ui->resetPushButton, &QPushButton::clicked, this, &TelecommandWidget::onResetClicked);
    connect(ui->standbyPushButton, &QPushButton::clicked, this, &TelecommandWidget::onStandbyClicked);
    connect(ui->coldStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onColdstartClicked);
    connect(ui->warmStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onWarmstartClicked);
    connect(ui->hotStartPushButton, &QPushButton::clicked, this, &TelecommandWidget::onHotstartClicked);
    connect(ui->statusPushButton, &QPushButton::clicked, this, &TelecommandWidget::onStatusClicked);
    connect(ui->connectPushButton, &QPushButton::clicked, this, &TelecommandWidget::onConnectClicked);

    connect(&m_telnetManager, &TelnetManager::txData, this, &TelecommandWidget::printText);
    connect(&m_telnetManager, &TelnetManager::rxData, this, &TelecommandWidget::printText);

    connect(&m_telnetManager, &TelnetManager::connected, this, &TelecommandWidget::statusConnected);
    connect(&m_telnetManager, &TelnetManager::disconnected, this, &TelecommandWidget::statusDisconnected);

    // Forward signals from TelnetManager.
    connect(&m_telnetManager, &TelnetManager::connected, this, &TelecommandWidget::connected);
    connect(&m_telnetManager, &TelnetManager::disconnected, this, &TelecommandWidget::disconnected);

    connect(&m_telnetManager, &TelnetManager::error, this, &TelecommandWidget::printError);

    // Load settings from last session.
    loadSettings();

    handleInputsChanged();
}

TelecommandWidget::~TelecommandWidget()
{
    m_telnetManager.disconnectTcp();

    QSettings settings;
    settings.beginGroup("TelecommandWidget");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    delete ui;
}

void TelecommandWidget::setAddress(QString addr_str)
{
    m_telnetManager.setAddress(addr_str);
    ui->addressLineEdit->setText(addr_str);
}

void TelecommandWidget::setPort(QString port_str)
{
    m_telnetManager.setPort(port_str);
    ui->portLineEdit->setText(port_str);
}

void TelecommandWidget::handleInputsChanged()
{
    if (ui->addressLineEdit->text().isEmpty() || ui->portLineEdit->text().isEmpty())
    {
        ui->connectPushButton->setEnabled(false);
        ui->connectPushButton->setStyleSheet(styleSheet());
    }
    else
    {
        ui->connectPushButton->setEnabled(true);
        ui->connectPushButton->setStyleSheet("background-color:#2ECC40;");
    }
}

void TelecommandWidget::clear()
{
    ui->plainTextEdit->clear();
}

void TelecommandWidget::onAddressEditTextchanged()
{
    emit inputsChanged();
}

void TelecommandWidget::onPortEditTextchanged()
{
    emit inputsChanged();
}

void TelecommandWidget::onResetClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::Reset);

    if (ui->reconnectCheckBox->isChecked())
    {
        m_timer.start();
    }

    if (ui->clearCheckBox->isChecked())
    {
        emit resetClicked();
    }
}

void TelecommandWidget::onStandbyClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::Standby);
}

void TelecommandWidget::onColdstartClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::ColdStart);
}

void TelecommandWidget::onWarmstartClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::WarmStart, getArgs());
}

void TelecommandWidget::onHotstartClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::HotStart, getArgs());
}

void TelecommandWidget::onStatusClicked()
{
    m_telnetManager.sendCommand(TelnetManager::Command::Status);
}

void TelecommandWidget::onConnectClicked()
{
    // What's the current state of the socket?
    QAbstractSocket::SocketState state = m_telnetManager.getState();

    if (state == QAbstractSocket::ConnectedState)
    {
        // The socket is connected, so let's disconnect now.
        m_telnetManager.sendCommand(TelnetManager::Command::Exit);
    }
    else if (state == QAbstractSocket::UnconnectedState)
    {
        // The socket is disconnected, so let's attempt a connection now.
        setAddress(ui->addressLineEdit->text());
        setPort(ui->portLineEdit->text());
        m_telnetManager.connectTcp();
    }
}

void TelecommandWidget::reconnect()
{
    // What's the current state of the socket?
    QAbstractSocket::SocketState state = m_telnetManager.getState();

    if (state != QAbstractSocket::ConnectedState && state != QAbstractSocket::ConnectingState)
    {
        // Attempt a connection
        m_telnetManager.connectTcp();
    }

    if (state == QAbstractSocket::ConnectedState)
    {
        m_timer.stop();
    }
}

void TelecommandWidget::statusConnected()
{
    // The connection was successful so let's save the address and port number.
    QSettings settings;
    settings.beginGroup("tcp_socket");
    settings.setValue("address", m_telnetManager.getAddress().toString());
    settings.setValue("port", m_telnetManager.getPort());
    settings.endGroup();

    qDebug() << "Settings Saved";

    // Disable the ip address and port number edit fields.
    ui->addressLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);

    // Enable the command buttons.
    ui->statusPushButton->setEnabled(true);
    ui->standbyPushButton->setEnabled(true);
    ui->resetPushButton->setEnabled(true);
    ui->coldStartPushButton->setEnabled(true);
    ui->warmStartPushButton->setEnabled(true);
    ui->hotStartPushButton->setEnabled(true);

    ui->connectPushButton->setText("Exit");
    ui->connectPushButton->setStyleSheet("background-color:#FF4136;");
}

void TelecommandWidget::statusDisconnected()
{
    // Enable the ip address and port number edit fields.
    ui->addressLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);

    // Disable the command buttons.
    ui->statusPushButton->setEnabled(false);
    ui->standbyPushButton->setEnabled(false);
    ui->resetPushButton->setEnabled(false);
    ui->coldStartPushButton->setEnabled(false);
    ui->warmStartPushButton->setEnabled(false);
    ui->hotStartPushButton->setEnabled(false);

    ui->connectPushButton->setText("Connect");
    ui->connectPushButton->setStyleSheet("background-color:#2ECC40;");
}

void TelecommandWidget::printText(QByteArray data)
{
    // Save current position of the vertical scroll bar.
    int scroll = ui->plainTextEdit->verticalScrollBar()->value();

    // Move cursor to the end.
    ui->plainTextEdit->moveCursor(QTextCursor::End);

    // Write text.
    QString text(data);
    ui->plainTextEdit->insertPlainText(text);

    // When finished writing restore scroll bar position or scroll to the end.
    if (ui->autoScrollCheckBox->isChecked())
    {
        // Scroll to the end.
        ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
    }
    else
    {
        // Restore saved scroll bar position.
        ui->plainTextEdit->verticalScrollBar()->setValue(scroll);
    }
}

void TelecommandWidget::loadSettings()
{
    m_settings.beginGroup("TelecommandWidget");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();

    m_settings.beginGroup("tcp_socket");
    setAddress(m_settings.value("address", "").toString());
    setPort(m_settings.value("port", "").toString());
    m_settings.endGroup();

    qDebug() << "Settings Loaded";
}

QString TelecommandWidget::getArgs()
{
    QString args = QString("%1 %2 %3 %4").arg(ui->dateTimeEdit->text(), ui->latitudeDoubleSpinBox->text(), ui->longitudeDoubleSpinBox->text(), ui->heightDoubleSpinBox->text());

    return args;
}

void TelecommandWidget::printError(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
}

void TelecommandWidget::saveSettings()
{
    m_settings.beginGroup("TelecommandWidget");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();

    m_settings.beginGroup("tcp_socket");
    m_settings.setValue("address", m_telnetManager.getAddress().toString());
    m_settings.setValue("port", m_telnetManager.getPort());
    m_settings.endGroup();

    qDebug() << "Settings Saved";
}
