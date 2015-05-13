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

    // Test construstors

    void unitByName_data();
    void unitByName();
    void unitBySymbol_data();
    void unitBySymbol();
    void parseUnit_data();
    void parseUnit();
    void dimensionLessUnit();
    void unitTypes_data();
    void unitTypes();
    void unitEquality_data();
    void unitEquality();

    void offset_data();
    void offset();
    void scale_data();
    void scale();
    void product_data();
    void product();
    void power_data();
    void power();
    void logarithm_data();
    void logarithm();

    void convert_data();
    void convert();
    // TODO: operation on invalid unit yields invalid units

private:
    UdUnitSystem *m_system;
};

UdUnits2Test::UdUnits2Test()
{
}

void UdUnits2Test::initTestCase()
{
    ut_set_error_message_handler(ut_ignore);
    m_system = UdUnitSystem::loadDatabase();
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
    if (unit.isValid())
        QVERIFY(unit.name() == name);
    else
        QVERIFY(unit.name() == QString(""));
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
    if (unit.isValid())
        QVERIFY(unit.symbol() == symbol);
    else
        QVERIFY(unit.symbol() == QString(""));
}

// TBD: Are we ok with null and empty strings leading the dimensionless unit one?
// TBD: See as well unitTypes(), dimensionless unit one is a product unit
void UdUnits2Test::parseUnit_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("validity");
    QTest::addColumn<QString>("definition");
    QTest::newRow("null string")  << QString()         << true  << QString("1");
    QTest::newRow("empty string") << QString("")       << true  << QString("1");
    QTest::newRow("bad string")   << QString("fbb^2")  << false << QString("");
    QTest::newRow("good string")  << QString("km.s^2") << true  << QString("1000 m·s²");
}

void UdUnits2Test::parseUnit()
{
    QFETCH(QString, string);
    QFETCH(bool, validity);
    QFETCH(QString, definition);
    UdUnit unit = m_system->unitFromString(string);
    QVERIFY(unit.isValid() == validity);
    QVERIFY(unit.format() == definition);
}

void UdUnits2Test::dimensionLessUnit()
{
    UdUnit unit = m_system->dimensionLessUnitOne();
    QVERIFY(unit.isValid() == true);
    QVERIFY(unit.isDimensionless() == true);
#if 0 // FIXME: this crashes
    unit = m_system->unitFromString("m");
    QVERIFY(unit.isValid() == true);
    QVERIFY(unit.isDimensionless() == false);
#else // But not this one
    UdUnit unit2 = m_system->unitFromString("m");
    QVERIFY(unit2.isValid() == true);
    QVERIFY(unit2.isDimensionless() == false);
#endif
}

void UdUnits2Test::unitTypes_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<UdUnit::UnitType>("type");
    QTest::newRow("null unit")      << QString()               << UdUnit::ProductUnit;
    QTest::newRow("empty unit")     << QString("")             << UdUnit::ProductUnit;
    QTest::newRow("unknown unit")   << QString("FBZ")          << UdUnit::NullUnit;
    QTest::newRow("basic unit")     << QString("m")            << UdUnit::BasicUnit;
    QTest::newRow("product unit")   << QString("m.s^-1")       << UdUnit::ProductUnit;
    QTest::newRow("galilean unit")  << QString("3.14 m")       << UdUnit::GalileanUnit;
    QTest::newRow("timestamp unit") << QString("s @ 1970T00")  << UdUnit::TimestampUnit;
    QTest::newRow("log unit")       << QString("log(re: 1mW)") << UdUnit::LogarithmicUnit;
}

void UdUnits2Test::unitTypes()
{
    QFETCH(QString, expression);
    QFETCH(UdUnit::UnitType, type);
    UdUnit unit = m_system->unitFromString(expression);
    QVERIFY(unit.type() == type);
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

void UdUnits2Test::offset_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<qreal>("offset");
    QTest::addColumn<QString>("output");
    QTest::newRow("K @ 273.15") << QString("kelvin") << 273.15 << QString("degC");
}

void UdUnits2Test::offset()
{
    QFETCH(QString, input);
    QFETCH(qreal, offset);
    QFETCH(QString, output);
    UdUnit inputUnit = m_system->unitFromString(input);
    UdUnit outputUnit = m_system->unitFromString(output);
    QVERIFY(inputUnit + offset == outputUnit);
    QVERIFY(offset + inputUnit == outputUnit);
    QVERIFY(outputUnit - offset == inputUnit);
    QVERIFY(offset - outputUnit == 0.0 - inputUnit); // TBD: add operator -(const UdUnit &rhs)
}

// TBD: invalid units
void UdUnits2Test::scale_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<qreal>("multiplier");
    QTest::addColumn<QString>("output");
    QTest::newRow("1000*g = 1 kg")  << QString("g")  << 1000.0 << "kg";
    QTest::newRow("0.001*g = 1 mg") << QString("g")  << 0.001  << "mg";
    QTest::newRow("1*kg = 1 kg")    << QString("kg") << 1.0    << "kg";
}

// TBD: invalid units
void UdUnits2Test::scale()
{
    QFETCH(QString, input);
    QFETCH(qreal, multiplier);
    QFETCH(QString, output);
    UdUnit inputUnit = m_system->unitFromString(input);
    UdUnit outputUnit = m_system->unitFromString(output);
    QVERIFY(inputUnit*multiplier == outputUnit);
    QVERIFY(multiplier*inputUnit == outputUnit);
    QVERIFY(outputUnit/multiplier == inputUnit);
    QVERIFY(multiplier/outputUnit == 1.0/inputUnit);
    QVERIFY((outputUnit/inputUnit).isDimensionless() == true);
    QVERIFY((inputUnit/outputUnit).isDimensionless() == true);
}

void UdUnits2Test::product_data()
{
    QTest::addColumn<QString>("input1");
    QTest::addColumn<QString>("input2");
    QTest::addColumn<QString>("output");
    QTest::newRow("m*s = 1 m.s") << QString("m")  << QString("s") << QString("m.s");
    QTest::newRow("X*Y = 1 ohm") << QString("s^-3.A^-2") << QString("m^2.kg") << QString("ohm");
}

void UdUnits2Test::product()
{
    QFETCH(QString, input1);
    QFETCH(QString, input2);
    QFETCH(QString, output);
    UdUnit inputUnit1 = m_system->unitFromString(input1);
    UdUnit inputUnit2 = m_system->unitFromString(input2);
    UdUnit outputUnit = m_system->unitFromString(output);
    QVERIFY(inputUnit1*inputUnit2 == outputUnit);
    QVERIFY(inputUnit1 == outputUnit/inputUnit2);
    QVERIFY(inputUnit2*inputUnit1 == outputUnit);
    QVERIFY(inputUnit2 == outputUnit/inputUnit1);
}

void UdUnits2Test::power_data()
{
    QTest::addColumn<QString>("base");
    QTest::addColumn<int>("power");
    QTest::addColumn<QString>("result");
    QTest::newRow("m by 3")   << QString("m")   << 3 << QString("m^3");
    QTest::newRow("ohm by 2") << QString("ohm") << 2 << QString("m^4.kg^2.s^-6.A^-4");
}

void UdUnits2Test::power()
{
    QFETCH(QString, base);
    QFETCH(int, power);
    QFETCH(QString, result);
    UdUnit baseUnit = m_system->unitFromString(base);
    UdUnit resultUnit = m_system->unitFromString(result);
    QVERIFY(baseUnit.raisedBy(power) == resultUnit);
    QVERIFY(resultUnit.rootedBy(power) == baseUnit);
}

void UdUnits2Test::logarithm_data()
{
    QTest::addColumn<qreal>("base");
    QTest::addColumn<QString>("reference");
    QTest::addColumn<QString>("result");
    QTest::newRow("W by 10")   << 10.0 << QString("W") << QString("lg(re 1 m^2.kg.s^-3)");
}

void UdUnits2Test::logarithm()
{
    QFETCH(qreal, base);
    QFETCH(QString, reference);
    QFETCH(QString, result);
    UdUnit referenceUnit = m_system->unitFromString(reference);
    UdUnit resultUnit = m_system->unitFromString(result);
    QVERIFY(referenceUnit.toLogarithmic(base) == resultUnit);
    // No log reciprocal, ie. power()
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
