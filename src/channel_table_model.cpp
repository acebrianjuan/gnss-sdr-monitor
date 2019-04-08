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
#include <QtGui>
#include <QList>
#include <string.h>

#include <QDebug>

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

            Gnss_Synchro channel = channels.at(channel_id);

            QString channel_signal = channels_signal.at(channel_id);


            boost::circular_buffer<double> channel_time_cbuf = channels_time.at(channel_id);
            boost::circular_buffer<double> channel_prompt_i_cbuf = channels_prompt_i.at(channel_id);
            boost::circular_buffer<double> channel_prompt_q_cbuf = channels_prompt_q.at(channel_id);
            boost::circular_buffer<double> channel_cn0_cbuf = channels_cn0.at(channel_id);
            boost::circular_buffer<double> channel_doppler_cbuf = channels_doppler.at(channel_id);


            QList<QVariant> channel_prompt_iq;
            QList<QVariant> channel_cn0;
            QList<QVariant> channel_doppler;


            for (int i = 0; i < channel_cn0_cbuf.size(); i++)
            {
                channel_prompt_iq << QPointF(channel_prompt_i_cbuf.at(i), channel_prompt_q_cbuf.at(i));
                channel_cn0 << QPointF(channel_time_cbuf.at(i), channel_cn0_cbuf.at(i));
                channel_doppler << QPointF(channel_time_cbuf.at(i), channel_doppler_cbuf.at(i));
            }


            if (role == Qt::DisplayRole)
            {
                switch (index.column())
                {
                case 0 :
                    return channel.Channel_ID;

                case 1 :
                    return channel_signal;

                case 2 :
                    return channel.PRN;

                case 3 :
                    return channel.Acq_doppler_hz;

                case 4:
                    return channel.Acq_delay_samples;

                case 5 :
                    return channel_prompt_iq;

                case 6 :
                    return channel_cn0;

                case 7 :
                    return channel_doppler;

                case 8 :
                    return channel.TOW_at_current_symbol_ms;

                case 9 :
                    return channel.Flag_valid_word;

                case 10 :
                    return channel.Pseudorange_m;
                }
            }
            else if (role == Qt::ToolTipRole)
            {
                switch (index.column())
                {
                case 0 :
                    return QVariant::Invalid;

                case 1 :
                    return QVariant::Invalid;

                case 2 :
                    return QVariant::Invalid;

                case 3 :
                    return QVariant::Invalid;

                case 4 :
                    return QVariant::Invalid;

                case 5 :
                    return QVariant::Invalid;

                case 6 :
                    return channel_cn0_cbuf.back();

                case 7 :
                    return channel_doppler_cbuf.back();

                case 8 :
                    return QVariant::Invalid;

                case 9 :
                    return QVariant::Invalid;

                case 10 :
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

QVariant Channel_Table_Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(section)
            {
            case 0 :
                return "CH";

            case 1 :
                return "Signal";

            case 2 :
                return "PRN";

            case 3 :
                return "ACQ Doppler [Hz]";

            case 4 :
                return "ACQ Code Phase [samples]";

            case 5 :
                return "Constellation";

            case 6 :
                return "C/N0 [dB-Hz]";

            case 7 :
                return "Doppler [Hz]";

            case 8 :
                return "TOW [ms]";

            case 9 :
                return "TLM";

            case 10 :
                return "Pseudorange [m]";
            }
        }
    }
    return QVariant::Invalid;
}

void Channel_Table_Model::populate_channels(std::vector<Gnss_Synchro> stocks)
{
    for (std::size_t i = 0; i < stocks.size(); i++)
    {
        populate_channel(stocks[i]);
    }
}

void Channel_Table_Model::populate_channel(Gnss_Synchro ch)
{
    if (ch.fs != 0) // Channel is valid.
    {

        if (channels.find(ch.Channel_ID) != channels.end()) // Channel exists.
        {
            /*
            int c1 = QString::compare(QString(channels.at(ch.Channel_ID).System),
                                      QString(ch.System), Qt::CaseInsensitive);
            int c2 = QString::compare(QString(channels.at(ch.Channel_ID).Signal),
                                      QString(ch.Signal), Qt::CaseInsensitive);
            */
            bool c3 = channels.at(ch.Channel_ID).PRN != ch.PRN;

            if (/* c1 != 0 || c2 != 0 || */ c3)
            {
                // Reset channel.
                clear_channel(ch.Channel_ID);
            }
        }
        /*
        else // Channel does not exist.
        {
            bool c1 = false;
            bool c2 = false;
            bool c3 = false;
            int ch_id;

            for (size_t i = 0; i < channels.size(); i++)
            {
                if (QString::compare(QString(channels[i].System), QString(ch.System)) == 0)
                {
                    c1 = true;
                }

                if (QString::compare(QString(channels[i].Signal), QString(ch.Signal)) == 0)
                {
                    c2 = true;
                }

                if (channels[i].PRN == ch.PRN)
                {
                    c3 = true;
                }

                if (c1 && c2 && c3)
                {
                    qDebug() << "Deleting Channel : " << ch_id << "\t" << "Replaced by Channel : " << ch.Channel_ID;
                    ch_id = channels[i].Channel_ID;
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

        channels[ch.Channel_ID] = ch;


        if (channels_time.find(ch.Channel_ID) == channels_time.end())
        {
            channels_time[ch.Channel_ID].resize(buffer_size);
            channels_time[ch.Channel_ID].clear();
        }
        channels_time[ch.Channel_ID].push_back(ch.RX_time);


        if (channels_prompt_i.find(ch.Channel_ID) == channels_prompt_i.end())
        {
            channels_prompt_i[ch.Channel_ID].resize(buffer_size);
            channels_prompt_i[ch.Channel_ID].clear();
        }
        channels_prompt_i[ch.Channel_ID].push_back(ch.Prompt_I);


        if (channels_prompt_q.find(ch.Channel_ID) == channels_prompt_q.end())
        {
            channels_prompt_q[ch.Channel_ID].resize(buffer_size);
            channels_prompt_q[ch.Channel_ID].clear();
        }
        channels_prompt_q[ch.Channel_ID].push_back(ch.Prompt_Q);


        if (channels_cn0.find(ch.Channel_ID) == channels_cn0.end())
        {
            channels_cn0[ch.Channel_ID].resize(buffer_size);
            channels_cn0[ch.Channel_ID].clear();
        }
        channels_cn0[ch.Channel_ID].push_back(ch.CN0_dB_hz);


        if (channels_doppler.find(ch.Channel_ID) == channels_doppler.end())
        {
            channels_doppler[ch.Channel_ID].resize(buffer_size);
            channels_doppler[ch.Channel_ID].clear();
        }
        channels_doppler[ch.Channel_ID].push_back(ch.Carrier_Doppler_hz);


        channels_signal[ch.Channel_ID] = get_signal_pretty_name(ch);


        if (channels.size() != map_size)
        {
            channels_id.push_back(ch.Channel_ID);
        }
    }
}

void Channel_Table_Model::clear_channel(int ch_id)
{
    channels_id.erase(std::remove(channels_id.begin(), channels_id.end(), ch_id), channels_id.end());
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

QString Channel_Table_Model::get_signal_pretty_name(Gnss_Synchro ch)
{
    QString system_name;

    if (ch.System != '\0')
    {
        if (ch.System == 'G')
        {
            system_name = QStringLiteral("GPS");
        }
        else if (ch.System == 'E')
        {
            system_name = QStringLiteral("Galileo");
        }

        if (map_signal_pretty_name.find(ch.Signal) != map_signal_pretty_name.end())
        {
            system_name.append(" ").append(map_signal_pretty_name.at(ch.Signal));
        }
    }

    return system_name;
}

QList<QVariant> Channel_Table_Model::get_list_from_cbuf(boost::circular_buffer<double> cbuf)
{
    QList<QVariant> list;

    for(size_t i = 0; i < cbuf.size(); i++)
    {
        list << cbuf.at(i);
    }

    return list;
}

int Channel_Table_Model::get_columns()
{
    return columns;
}

void Channel_Table_Model::set_buffer_size()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    int size = settings.value("buffer_size", DEFAULT_BUFFER_SIZE).toInt();
    settings.endGroup();

    buffer_size = size;
    clear_channels();
}

int Channel_Table_Model::get_channel_id(int row)
{
    return channels_id.at(row);
}
