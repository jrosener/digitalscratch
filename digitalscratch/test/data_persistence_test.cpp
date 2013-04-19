#include <QtTest>
#include <data_persistence_test.h>
#include <data_persistence.h>
#include <singleton.h>

Data_persistence_Test::Data_persistence_Test()
{
}

void Data_persistence_Test::initTestCase()
{
    // Get/create a data persistence static instance.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Init DB.
    QVERIFY2(data_persist->is_initialized == true, "DB initialized");
}

void Data_persistence_Test::cleanupTestCase()
{
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    data_persist->close_db();
}
