/*!
 * \file channel_table_model.cpp
 * \brief Implementation of a model for storing the channels information
 * in a table-like data structure.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2018  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
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
 * -------------------------------------------------------------------------
 */

#include "channel_table_model.h"
#include <QDebug>
#include <QList>
#include <QtGui>
#include <string.h>

#define DEFAULT_BUFFER_SIZE 1000

Channel_Table_Model::Channel_Table_Model()
{
    map_signal_pretty_name["1C"] = "L1 C/A";
    map_signal_pretty_name["1B"] = "E1";
    map_signal_pretty_name["1G"] = "L1 C/A";
    map_signal_pretty_name["2S"] = "L2C";
    map_signal_pretty_name["2G"] = "L2 C/A";
    map_signal_pretty_name["5X"] = "E5a";
    map_signal_pretty_name["L5"] = "L5";

    columns = 11;
    buffer_size = DEFAULT_BUFFER_SIZE;
}

void Channel_Table_Model::update()
{
    beginResetModel();
    endResetModel();
}

int Channel_Table_Model::rowCount(const QModelIndex &parent) const
{
    return channels.size();
}

int Channel_Table_Model::columnCount(const QModelIndex &parent) const
{
    return columns;
}

QVariant Channel_Table_Model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
        {
            try
                {
                    int channel_id = channels_id.at(index.row());

                    gnss_sdr::GnssSynchro channel = channels.at(channel_id);

                    QString channel_signal = channels_signal.at(channel_id);

                    boost::circular_buffer<double> channel_time_cbuf =
                        channels_time.at(channel_id);
                    boost::circular_buffer<double> channel_prompt_i_cbuf =
                        channels_prompt_i.at(channel_id);
                    boost::circular_buffer<double> channel_prompt_q_cbuf =
                        channels_prompt_q.at(channel_id);
                    boost::circular_buffer<double> channel_cn0_cbuf =
                        channels_cn0.at(channel_id);
                    boost::circular_buffer<double> channel_doppler_cbuf =
                        channels_doppler.at(channel_id);

                    QList<QVariant> channel_prompt_iq;
                    QList<QVariant> channel_cn0;
                    QList<QVariant> channel_doppler;

                    for (int i = 0; i < channel_cn0_cbuf.size(); i++)
                        {
                            channel_prompt_iq << QPointF(channel_prompt_i_cbuf.at(i),
                                channel_prompt_q_cbuf.at(i));
                            channel_cn0 << QPointF(channel_time_cbuf.at(i), channel_cn0_cbuf.at(i));
                            channel_doppler << QPointF(channel_time_cbuf.at(i),
                                channel_doppler_cbuf.at(i));
                        }

                    if (role == Qt::DisplayRole)
                        {
                            switch (index.column())
                                {
                                case 0:
                                    return channel.channel_id();

                                case 1:
                                    return channel_signal;

                                case 2:
                                    return channel.prn();

                                case 3:
                                    return channel.acq_doppler_hz();

                                case 4:
                                    return channel.acq_delay_samples();

                                case 5:
                                    return channel_prompt_iq;

                                case 6:
                                    return channel_cn0;

                                case 7:
                                    return channel_doppler;

                                case 8:
                                    return channel.tow_at_current_symbol_ms();

                                case 9:
                                    return channel.flag_valid_word();

                                case 10:
                                    return channel.pseudorange_m();
                                }
                        }
                    else if (role == Qt::ToolTipRole)
                        {
                            switch (index.column())
                                {
                                case 0:
                                    return QVariant::Invalid;

                                case 1:
                                    return QVariant::Invalid;

                                case 2:
                                    return QVariant::Invalid;

                                case 3:
                                    return QVariant::Invalid;

                                case 4:
                                    return QVariant::Invalid;

                                case 5:
                                    return QVariant::Invalid;

                                case 6:
                                    return channel_cn0_cbuf.back();

                                case 7:
                                    return channel_doppler_cbuf.back();

                                case 8:
                                    return QVariant::Invalid;

                                case 9:
                                    return QVariant::Invalid;

                                case 10:
                                    return QVariant::Invalid;
                                }
                        }
                }
            catch (const std::exception &ex)
                {
                    qDebug() << ex.what();
                    return QVariant::Invalid;
                }
        }
    else if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    return QVariant::Invalid;
}

QVariant Channel_Table_Model::headerData(int section,
    Qt::Orientation orientation,
    int role) const
{
    if (role == Qt::DisplayRole)
        {
            if (orientation == Qt::Horizontal)
                {
                    switch (section)
                        {
                        case 0:
                            return "CH";

                        case 1:
                            return "Signal";

                        case 2:
                            return "PRN";

                        case 3:
                            return "ACQ Doppler [Hz]";

                        case 4:
                            return "ACQ Code Phase [samples]";

                        case 5:
                            return "Constellation";

                        case 6:
                            return "C/N0 [dB-Hz]";

                        case 7:
                            return "Doppler [Hz]";

                        case 8:
                            return "TOW [ms]";

                        case 9:
                            return "TLM";

                        case 10:
                            return "Pseudorange [m]";
                        }
                }
        }
    return QVariant::Invalid;
}

void Channel_Table_Model::populate_channels(
    const gnss_sdr::Observables *stocks)
{
    for (std::size_t i = 0; i < stocks->observable_size(); i++)
        {
            populate_channel(&stocks->observable(i));
        }
}

void Channel_Table_Model::populate_channel(const gnss_sdr::GnssSynchro *ch)
{
    if (ch->fs() != 0)  // Channel is valid.
        {
            if (channels.find(ch->channel_id()) != channels.end())  // Channel exists.
                {
                    /*
                    int c1 = QString::compare(QString(channels.at(ch->channel_id()).system()),
                                QString(ch.System), Qt::CaseInsensitive);
                    int c2 = QString::compare(QString(channels.at(ch->channel_id()).signal()),
                                QString(ch->signal()), Qt::CaseInsensitive);
                    */
                    bool c3 = channels.at(ch->channel_id()).prn() != ch->prn();

                    if (/* c1 != 0 || c2 != 0 || */ c3)
                        {
                            // Reset channel.
                            clear_channel(ch->channel_id());
                        }
                }
            /*
            else  // Channel does not exist.
                {
                    bool c1 = false;
                    bool c2 = false;
                    bool c3 = false;
                    int ch_id;

                    for (size_t i = 0; i < channels.size(); i++)
                        {
                            if (QString::compare(QString(channels[i].system()),
                                    QString(ch.System)) == 0)
                                {
                                    c1 = true;
                                }

                            if (QString::compare(QString(channels[i].signal()),
                                    QString(ch->signal())) == 0)
                                {
                                    c2 = true;
                                }

                            if (channels[i].PRN == ch->prn())
                                {
                                    c3 = true;
                                }

                            if (c1 && c2 && c3)
                                {
                                    qDebug() << "Deleting Channel : " << ch_id << "\t"
                                             << "Replaced by Channel : " << ch->channel_id();
                                    ch_id =
                                        channels[i].channel_id();
                                    clear_channel(ch_id);
                                    break;
                                }
                            else
                                {
                                    c1 = false;
                                    c2 = false;
                                    c3 = false;
                                }
                        }
                }
            */

            size_t map_size = channels.size();

            channels[ch->channel_id()] = *ch;

            if (channels_time.find(ch->channel_id()) == channels_time.end())
                {
                    channels_time[ch->channel_id()].resize(buffer_size);
                    channels_time[ch->channel_id()].clear();
                }
            channels_time[ch->channel_id()].push_back(ch->rx_time());

            if (channels_prompt_i.find(ch->channel_id()) == channels_prompt_i.end())
                {
                    channels_prompt_i[ch->channel_id()].resize(buffer_size);
                    channels_prompt_i[ch->channel_id()].clear();
                }
            channels_prompt_i[ch->channel_id()].push_back(ch->prompt_i());

            if (channels_prompt_q.find(ch->channel_id()) == channels_prompt_q.end())
                {
                    channels_prompt_q[ch->channel_id()].resize(buffer_size);
                    channels_prompt_q[ch->channel_id()].clear();
                }
            channels_prompt_q[ch->channel_id()].push_back(ch->prompt_q());

            if (channels_cn0.find(ch->channel_id()) == channels_cn0.end())
                {
                    channels_cn0[ch->channel_id()].resize(buffer_size);
                    channels_cn0[ch->channel_id()].clear();
                }
            channels_cn0[ch->channel_id()].push_back(ch->cn0_db_hz());

            if (channels_doppler.find(ch->channel_id()) == channels_doppler.end())
                {
                    channels_doppler[ch->channel_id()].resize(buffer_size);
                    channels_doppler[ch->channel_id()].clear();
                }
            channels_doppler[ch->channel_id()].push_back(ch->carrier_doppler_hz());

            channels_signal[ch->channel_id()] = get_signal_pretty_name(ch);

            if (channels.size() != map_size)
                {
                    channels_id.push_back(ch->channel_id());
                }
        }
}

void Channel_Table_Model::clear_channel(int ch_id)
{
    channels_id.erase(std::remove(channels_id.begin(), channels_id.end(), ch_id),
        channels_id.end());
    channels.erase(ch_id);
    channels_signal.erase(ch_id);
    channels_time.erase(ch_id);
    channels_prompt_i.erase(ch_id);
    channels_prompt_q.erase(ch_id);
    channels_cn0.erase(ch_id);
    channels_doppler.erase(ch_id);
}

void Channel_Table_Model::clear_channels()
{
    channels_id.clear();
    channels.clear();
    channels_signal.clear();
    channels_time.clear();
    channels_prompt_i.clear();
    channels_prompt_q.clear();
    channels_cn0.clear();
    channels_doppler.clear();
}

QString
Channel_Table_Model::get_signal_pretty_name(const gnss_sdr::GnssSynchro *ch)
{
    QString system_name;

    if (!ch->system().empty())
        {
            if (ch->system() == "G")
                {
                    system_name = QStringLiteral("GPS");
                }
            else if (ch->system() == "E")
                {
                    system_name = QStringLiteral("Galileo");
                }

            if (map_signal_pretty_name.find(ch->signal()) !=
                map_signal_pretty_name.end())
                {
                    system_name.append(" ").append(map_signal_pretty_name.at(ch->signal()));
                }
        }

    return system_name;
}

QList<QVariant>
Channel_Table_Model::get_list_from_cbuf(boost::circular_buffer<double> cbuf)
{
    QList<QVariant> list;

    for (size_t i = 0; i < cbuf.size(); i++)
        {
            list << cbuf.at(i);
        }

    return list;
}

int Channel_Table_Model::get_columns() { return columns; }

void Channel_Table_Model::set_buffer_size()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    int size = settings.value("buffer_size", DEFAULT_BUFFER_SIZE).toInt();
    settings.endGroup();

    buffer_size = size;
    clear_channels();
}

int Channel_Table_Model::get_channel_id(int row) { return channels_id.at(row); }
