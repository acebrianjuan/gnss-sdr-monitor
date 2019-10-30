/*!
 * \file channel_table_model.cpp
 * \brief Implementation of a model for storing the channels information
 * in a table-like data structure.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
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


#include "channel_table_model.h"
#include <QDebug>
#include <QList>
#include <QtGui>
#include <string.h>

#define DEFAULT_BUFFER_SIZE 1000

/*!
 Constructs an instance of a table model.
 */
ChannelTableModel::ChannelTableModel()
{
    m_mapSignalPrettyName["1C"] = "L1 C/A";
    m_mapSignalPrettyName["1B"] = "E1";
    m_mapSignalPrettyName["1G"] = "L1 C/A";
    m_mapSignalPrettyName["2S"] = "L2C";
    m_mapSignalPrettyName["2G"] = "L2 C/A";
    m_mapSignalPrettyName["5X"] = "E5a";
    m_mapSignalPrettyName["L5"] = "L5";

    m_columns = 11;
    m_bufferSize = DEFAULT_BUFFER_SIZE;
}

/*!
 Triggers a reset of the table model which casues the views to be repainted.
 */
void ChannelTableModel::update()
{
    beginResetModel();
    endResetModel();
}

int ChannelTableModel::rowCount(const QModelIndex &parent) const
{
    return m_channels.size();
}

int ChannelTableModel::columnCount(const QModelIndex &parent) const
{
    return m_columns;
}

QVariant ChannelTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::DecorationRole)
    {
        try
        {
            int channel_id = m_channelsId.at(index.row());

            gnss_sdr::GnssSynchro channel = m_channels.at(channel_id);

            QString channel_signal = m_channelsSignal.at(channel_id);

            boost::circular_buffer<double> channel_time_cbuf =
                m_channelsTime.at(channel_id);
            boost::circular_buffer<double> channel_prompt_i_cbuf =
                m_channelsPromptI.at(channel_id);
            boost::circular_buffer<double> channel_prompt_q_cbuf =
                m_channelsPromptQ.at(channel_id);
            boost::circular_buffer<double> channel_cn0_cbuf =
                m_channelsCn0.at(channel_id);
            boost::circular_buffer<double> channel_doppler_cbuf =
                m_channelsDoppler.at(channel_id);

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
            else if (index.column() == 1 && role == Qt::DecorationRole)
            {
                if (channel.system() == "G")
                {
                    return QIcon(":/images/flag-us.png");
                }
                else if (channel.system() == "R")
                {
                    return QIcon(":/images/flag-ru.png");
                }
                else if (channel.system() == "E")
                {
                    return QIcon(":/images/flag-eu.png");
                }
                else if (channel.system() == "C")
                {
                    return QIcon(":/images/flag-cn.png");
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

QVariant ChannelTableModel::headerData(int section,
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

/*!
 Populates the internal data structures of the table model with the data of the \a stocks collection of GnssSynchro objects.
 Internally, this function calls populateChannel() on each individual GnssSynchro object in the collection.
 */
void ChannelTableModel::populateChannels(const gnss_sdr::Observables *stocks)
{
    for (std::size_t i = 0; i < stocks->observable_size(); i++)
    {
        populateChannel(&stocks->observable(i));
    }
}

/*!
 Populates the internal data structures of the table model with the data of the \a ch GnssSynchro object.
 */
void ChannelTableModel::populateChannel(const gnss_sdr::GnssSynchro *ch)
{
    // Check if channel is valid, if not, do nothing.
    if (ch->fs() != 0)
    {
        // Channel is valid, now check if it exists in the map of channels.
        if (m_channels.find(ch->channel_id()) != m_channels.end())
        {
            // Channel exists, now check if its PRN is the same.
            if (m_channels.at(ch->channel_id()).prn() != ch->prn())
            {
                // PRN has changed so reset the channel.
                clearChannel(ch->channel_id());
            }
        }

        // Check the size of the map of GnssSynchro objects before adding new data.
        size_t map_size = m_channels.size();

        // Add the new GnssSynchro object to the map.
        m_channels[ch->channel_id()] = *ch;

        // Time.
        // Check if channel exists in the map of time data.
        if (m_channelsTime.find(ch->channel_id()) == m_channelsTime.end())
        {
            // Channel does not exist so make room for it.
            m_channelsTime[ch->channel_id()].resize(m_bufferSize);
            m_channelsTime[ch->channel_id()].clear();
        }
        // Populate map with new time data.
        m_channelsTime[ch->channel_id()].push_back(ch->rx_time());

        // In-phase prompt component.
        // Check if channel exists in the map of in-phase component data.
        if (m_channelsPromptI.find(ch->channel_id()) == m_channelsPromptI.end())
        {
            // Channel does not exist so make room for it.
            m_channelsPromptI[ch->channel_id()].resize(m_bufferSize);
            m_channelsPromptI[ch->channel_id()].clear();
        }
        // Populate map with new in-phase component data.
        m_channelsPromptI[ch->channel_id()].push_back(ch->prompt_i());

        // Quadrature prompt component.
        // Check if channel exists in the map of quadrature component data.
        if (m_channelsPromptQ.find(ch->channel_id()) == m_channelsPromptQ.end())
        {
            // Channel does not exist so make room for it.
            m_channelsPromptQ[ch->channel_id()].resize(m_bufferSize);
            m_channelsPromptQ[ch->channel_id()].clear();
        }
        // Populate map with new quadrature component data.
        m_channelsPromptQ[ch->channel_id()].push_back(ch->prompt_q());

        // CN0.
        // Check if channel exists in the map of CN0 data.
        if (m_channelsCn0.find(ch->channel_id()) == m_channelsCn0.end())
        {
            // Channel does not exist so make room for it.
            m_channelsCn0[ch->channel_id()].resize(m_bufferSize);
            m_channelsCn0[ch->channel_id()].clear();
        }
        // Populate map with new CN0 data.
        m_channelsCn0[ch->channel_id()].push_back(ch->cn0_db_hz());

        // Doppler.
        // Check if channel exists in the map of Doppler data.
        if (m_channelsDoppler.find(ch->channel_id()) == m_channelsDoppler.end())
        {
            // Channel does not exist so make room for it.
            m_channelsDoppler[ch->channel_id()].resize(m_bufferSize);
            m_channelsDoppler[ch->channel_id()].clear();
        }
        // Populate map with new Doppler data.
        m_channelsDoppler[ch->channel_id()].push_back(ch->carrier_doppler_hz());

        // Signal name.
        // Populate map with new signal name.
        m_channelsSignal[ch->channel_id()] = getSignalPrettyName(ch);

        // Check the size of the map of GnssSynchro objects after adding new data.
        if (m_channels.size() != map_size)
        {
            // Map size has changed so record the new channel number in the vector of channel IDs.
            m_channelsId.push_back(ch->channel_id());
        }
    }
}

/*!
 Clears the data of a single channel specified by \a ch_id from the table model.
 */
void ChannelTableModel::clearChannel(int ch_id)
{
    m_channelsId.erase(std::remove(m_channelsId.begin(), m_channelsId.end(), ch_id),
        m_channelsId.end());
    m_channels.erase(ch_id);
    m_channelsSignal.erase(ch_id);
    m_channelsTime.erase(ch_id);
    m_channelsPromptI.erase(ch_id);
    m_channelsPromptQ.erase(ch_id);
    m_channelsCn0.erase(ch_id);
    m_channelsDoppler.erase(ch_id);
}

/*!
 Clears the data of all channels from the table model.
 */
void ChannelTableModel::clearChannels()
{
    m_channelsId.clear();
    m_channels.clear();
    m_channelsSignal.clear();
    m_channelsTime.clear();
    m_channelsPromptI.clear();
    m_channelsPromptQ.clear();
    m_channelsCn0.clear();
    m_channelsDoppler.clear();
}

/*!
 Gets the descriptive string formed by the combination of the GNSS system and signal name for a given \a ch GnssSynchro object.
 */
QString ChannelTableModel::getSignalPrettyName(const gnss_sdr::GnssSynchro *ch)
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

        if (m_mapSignalPrettyName.find(ch->signal()) != m_mapSignalPrettyName.end())
        {
            system_name.append(" ").append(m_mapSignalPrettyName.at(ch->signal()));
        }
    }

    return system_name;
}

/*!
 Gets a list from a circular buffer \a cbuf.
 */
QList<QVariant> ChannelTableModel::getListFromCbuf(boost::circular_buffer<double> cbuf)
{
    QList<QVariant> list;

    for (size_t i = 0; i < cbuf.size(); i++)
    {
        list << cbuf.at(i);
    }

    return list;
}

/*!
 Gets the number of columns of the table model.
 */
int ChannelTableModel::getColumns()
{
    return m_columns;
}

/*!
 Sets the size of the internal circular buffers that store the data of the table model.
 */
void ChannelTableModel::setBufferSize()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    int size = settings.value("buffer_size", DEFAULT_BUFFER_SIZE).toInt();
    settings.endGroup();

    m_bufferSize = size;
    clearChannels();
}

/*!
 Gets the id number of the channel occupying the queried \a row of the table model.
 */
int ChannelTableModel::getChannelId(int row)
{
    return m_channelsId.at(row);
}
