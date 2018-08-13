#ifndef CHANNEL_TABLE_MODEL_H
#define CHANNEL_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <boost/circular_buffer.hpp>
#include "gnss_synchro.h"

class Channel_Table_Model : public QAbstractTableModel
{
public:
    Channel_Table_Model();

    void populate_channels(std::vector<Gnss_Synchro> stocks);
    void populate_channel(Gnss_Synchro ch);
    void clear_channel(int ch_id);
    void clear_channels();
    QString get_signal_pretty_name(Gnss_Synchro ch);
    QList<QVariant> get_list_from_cbuf(boost::circular_buffer<double> cbuf);
    int get_columns();
    void set_buffer_size();

    // List of virual functions that must be implemented in a read-only table model.
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

};

#endif // CHANNEL_TABLE_MODEL_H
