/*!
 * \file channel_table_model.h
 * \brief Interface of a model for storing the channels information
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


#ifndef CHANNEL_TABLE_MODEL_H
#define CHANNEL_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <boost/circular_buffer.hpp>
#include "gnss_synchro.h"

class Channel_Table_Model : public QAbstractTableModel
{
public:
    Channel_Table_Model();

    void update();

    void populate_channels(std::vector<Gnss_Synchro> stocks);
    void populate_channel(Gnss_Synchro ch);
    void clear_channel(int ch_id);
    void clear_channels();
    QString get_signal_pretty_name(Gnss_Synchro ch);
    QList<QVariant> get_list_from_cbuf(boost::circular_buffer<double> cbuf);
    int get_columns();
    void set_buffer_size();
    int get_channel_id(int row);

    // List of virtual functions that must be implemented in a read-only table model.
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


protected:
    int columns;
    int buffer_size;
    std::vector<Gnss_Synchro> stocks;

    std::vector<int> channels_id;
    std::map<int, Gnss_Synchro> channels;
    std::map<int, QString> channels_signal;
    std::map<int, boost::circular_buffer<double>> channels_time;
    std::map<int, boost::circular_buffer<double>> channels_prompt_i;
    std::map<int, boost::circular_buffer<double>> channels_prompt_q;
    std::map<int, boost::circular_buffer<double>> channels_cn0;
    std::map<int, boost::circular_buffer<double>> channels_doppler;

public slots:
    Gnss_Synchro get_channel_data(int key);

private:
    std::map<std::string, QString> map_signal_pretty_name;
};

#endif // CHANNEL_TABLE_MODEL_H
