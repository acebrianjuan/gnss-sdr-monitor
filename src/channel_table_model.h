/*!
 * \file channel_table_model.h
 * \brief Interface of a model for storing the channels information
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


#ifndef GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_
#define GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_

#include "gnss_synchro.pb.h"
#include <boost/circular_buffer.hpp>
#include <QAbstractTableModel>

class ChannelTableModel : public QAbstractTableModel
{
public:
    ChannelTableModel();

    void update();

    void populateChannels(const gnss_sdr::Observables *m_stocks);
    void populateChannel(const gnss_sdr::GnssSynchro *ch);
    void clearChannel(int ch_id);
    void clearChannels();
    QString getSignalPrettyName(const gnss_sdr::GnssSynchro *ch);
    QList<QVariant> getListFromCbuf(boost::circular_buffer<double> cbuf);
    int getColumns();
    void setBufferSize();
    int getChannelId(int row);

    // List of virtual functions that must be implemented in a read-only table model.
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    gnss_sdr::GnssSynchro getChannelData(int key);

protected:
    int m_columns;
    int m_bufferSize;
    gnss_sdr::Observables m_stocks;

    std::vector<int> m_channelsId;
    std::map<int, gnss_sdr::GnssSynchro> m_channels;
    std::map<int, QString> m_channelsSignal;
    std::map<int, boost::circular_buffer<double>> m_channelsTime;
    std::map<int, boost::circular_buffer<double>> m_channelsPromptI;
    std::map<int, boost::circular_buffer<double>> m_channelsPromptQ;
    std::map<int, boost::circular_buffer<double>> m_channelsCn0;
    std::map<int, boost::circular_buffer<double>> m_channelsDoppler;

private:
    std::map<std::string, QString> m_mapSignalPrettyName;
};

#endif  // GNSS_SDR_MONITOR_CHANNEL_TABLE_MODEL_H_
