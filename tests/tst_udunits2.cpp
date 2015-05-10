#include <QString>
#include <QtTest>

#include "qudunit.h"

class UdUnits2Test : public QObject
{
    Q_OBJECT

public:
    UdUnits2Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void unitByName_data();
    void unitByName();
    void unitBySymbol_data();
    void unitBySymbol();
    void unitFromString_data();
    void unitFromString();
    void dimensionLessUnit();
    void unitEquality_data();
    void unitEquality();
    void upscaling_data();
    void upscaling();
    void downscaling_data();
    void downscaling();
    void multiplying_data();
    void multiplying();
    void dividing_data();
    void dividing();
    void convert_data();
    void convert();

private:
    UdUnitSystem *m_system;
};

UdUnits2Test::UdUnits2Test()
{
}

void UdUnits2Test::initTestCase()
{
    ut_set_error_message_handler(ut_ignore);
    m_system = new UdUnitSystem();
    QVERIFY(m_system->isValid());
}

void UdUnits2Test::cleanupTestCase()
{
    delete m_system;
}

void UdUnits2Test::init()
{
}

void UdUnits2Test::cleanup()
{
}

void UdUnits2Test::unitByName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("validity");
    QTest::newRow("null name")    << QString()            << false;
    QTest::newRow("empty name")   << QString("")          << false;
    QTest::newRow("unknown name") << QString("foobarbaz") << false;
    QTest::newRow("known name")   << QString("meter")     << true;
}

void UdUnits2Test::unitByName()
{
    QFETCH(QString, name);
    QFETCH(bool, validity);
    UdUnit unit = m_system->unitByName(name);
    QVERIFY(unit.isValid() == validity);
}

void UdUnits2Test::unitBySymbol_data()
{
    QTest::addColumn<QString>("symbol");
    QTest::addColumn<bool>("validity");
    QTest::newRow("null symbol")    << QString()      << false;
    QTest::newRow("empty symbol")   << QString("")    << false;
    QTest::newRow("unknown symbol") << QString("fbb") << false;
    QTest::newRow("known symbol")   << QString("m")   << true;
}

void UdUnits2Test::unitBySymbol()
{
    QFETCH(QString, symbol);
    QFETCH(bool, validity);
    UdUnit unit = m_system->unitBySymbol(symbol);
    QVERIFY(unit.isValid() == validity);
}

// TBD: Are we ok with null and empty strings leading the dimensionless unit one?
void UdUnits2Test::unitFromString_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("validity");
    QTest::addColumn<QString>("definition");
    QTest::newRow("null string")  << QString()         << true  << QString("1");
    QTest::newRow("empty string") << QString("")       << true  << QString("1");
    QTest::newRow("bad string")   << QString("fbb^2")  << false << QString("");
    QTest::newRow("good string")  << QString("km.s^2") << true  << QString("1000 meter·second²");
}

void UdUnits2Test::unitFromString()
{
    QFETCH(QString, string);
    QFETCH(bool, validity);
    QFETCH(QString, definition);
    UdUnit unit = m_system->unitFromString(string);
    QVERIFY(unit.isValid() == validity);
    QVERIFY(unit.toString() == definition);
}

void UdUnits2Test::dimensionLessUnit()
{
    UdUnit unit = m_system->dimensionLessUnitOne();
    QVERIFY(unit.isValid() == true);
}

void UdUnits2Test::unitEquality_data()
{
    QTest::addColumn<QString>("symbol1");
    QTest::addColumn<QString>("symbol2");
    QTest::addColumn<bool>("equality");
    QTest::newRow("same valid units")        << QString("m")   << QString("m")   << true;
    QTest::newRow("same invalid units")      << QString("FBZ") << QString("FBZ") << true;
    QTest::newRow("same null units")         << QString()      << QString()      << true;
    QTest::newRow("different invalid units") << QString("fbz") << QString("zbf") << true;
    QTest::newRow("different valid units")   << QString("m")   << QString("A")   << false;
}

void UdUnits2Test::unitEquality()
{
    QFETCH(QString, symbol1);
    QFETCH(QString, symbol2);
    QFETCH(bool, equality);
    UdUnit unit1 = m_system->unitBySymbol(symbol1);
    UdUnit unit2 = m_system->unitBySymbol(symbol2);
    QVERIFY((unit1 == unit2) == equality);
    QVERIFY((unit1 != unit2) != equality);
}

// TBD: invalid units
void UdUnits2Test::upscaling_data()
{
    QTest::addColumn<QString>("symbol");
    QTest::addColumn<qreal>("multiplier");
    QTest::addColumn<QString>("result");
    QTest::newRow("1000*g = 1 kg")  << QString("g")  << 1000.0 << "kilogram";
    QTest::newRow("0.001*g = 1 ug") << QString("g")  << 0.001  << "1e-06 kilogram";
    QTest::newRow("1*kg = 1 kg")    << QString("kg") << 1.0    << "kilogram";
}

// TBD: invalid units
void UdUnits2Test::upscaling()
{
    QFETCH(QString, symbol);
    QFETCH(qreal, multiplier);
    QFETCH(QString, result);
    UdUnit unit = m_system->unitBySymbol(symbol);
    QVERIFY((unit*multiplier).toString() == result);
    QVERIFY((multiplier*unit).toString() == result);
}

// TBD: invalid units
void UdUnits2Test::downscaling_data()
{
    QTest::addColumn<QString>("symbol");
    QTest::addColumn<qreal>("divider");
    QTest::addColumn<QString>("result");
    QTest::newRow("g/1000 = 1 ug")  << QString("g")  << 1000.0 << "1e-06 kilogram";
    QTest::newRow("g/0.001 = 1 kg") << QString("g")  << 0.001  << "kilogram";
    QTest::newRow("kg/1 = 1 kg")    << QString("kg") << 1.0    << "kilogram";
}

// TBD: invalid units
void UdUnits2Test::downscaling()
{
    QFETCH(QString, symbol);
    QFETCH(qreal, divider);
    QFETCH(QString, result);
    UdUnit unit = m_system->unitBySymbol(symbol);
    QVERIFY((unit/divider).toString() == result);
}

void UdUnits2Test::multiplying_data()
{
    QTest::addColumn<QString>("symbol1");
    QTest::addColumn<QString>("symbol2");
    QTest::addColumn<QString>("result");
    QTest::newRow("m*s = 1 m.s")  << QString("m")  << QString("s") << QString("meter·second");
}

void UdUnits2Test::multiplying()
{
    QFETCH(QString, symbol1);
    QFETCH(QString, symbol2);
    QFETCH(QString, result);
    UdUnit unit1 = m_system->unitBySymbol(symbol1);
    UdUnit unit2 = m_system->unitBySymbol(symbol2);
    QVERIFY((unit1*unit2).toString() == result);
    QVERIFY((unit2*unit1).toString() == result);
}

void UdUnits2Test::dividing_data()
{
    QTest::addColumn<QString>("symbol1");
    QTest::addColumn<QString>("symbol2");
    QTest::addColumn<QString>("result");
    QTest::newRow("m/s = 1 m.s⁻¹")  << QString("m")  << QString("s") << QString("meter·second⁻¹");
}

void UdUnits2Test::dividing()
{
    QFETCH(QString, symbol1);
    QFETCH(QString, symbol2);
    QFETCH(QString, result);
    UdUnit unit1 = m_system->unitBySymbol(symbol1);
    UdUnit unit2 = m_system->unitBySymbol(symbol2);
    QVERIFY((unit1/unit2).toString() == result);
}

void UdUnits2Test::convert_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("to");
    QTest::addColumn<bool>("validity");
    QTest::addColumn<qreal>("value");
    QTest::addColumn<qreal>("result");
    QTest::newRow("valid")   << QString("m/s") << QString("km/h")  << true  << 1000.0/3600.0 << 1.0;
    QTest::newRow("invalid") << QString("m/s") << QString("m/s^2") << false << 0.0           << 0.0;
}

void UdUnits2Test::convert()
{
    QFETCH(QString, from);
    QFETCH(QString, to);
    QFETCH(bool, validity);
    QFETCH(qreal, value);
    QFETCH(qreal, result);
    UdUnit ufrom = m_system->unitFromString(from);
    UdUnit uto = m_system->unitFromString(to);
    UdUnitConverter converter(ufrom, uto);
    QVERIFY(converter.isValid() == validity);
    if (validity)
        QVERIFY(converter.convert(value) == result);
}


QTEST_APPLESS_MAIN(UdUnits2Test)

#include "tst_udunits2.moc"
