#include "qudunit.h"

#include <QDebug>


/*!
 * \class UdUnitSystem
 * \ingroup index
 * \preliminary
 * \brief The UdUnitSystem class represents a unit-system.
 *
 * A unit-system is a set of units that are all defined in terms of the same
 * set of base units. In the SI system of units, for example, the base units
 * are the meter, kilogram, second, ampere, kelvin, mole, and candela.
 * (For definitions of these base units, see \l {http://physics.nist.gov/cuu/Units/current.html}.)
 *
 * In the \l {http://www.unidata.ucar.edu/software/udunits/}{UDUNITS-2 package},
 * every accessible unit belongs to one and only one unit-system.
 * It is not possible to convert numeric values between units of different unit-systems.
 * Similarly, units belonging to different unit-systems always compare unequal.
 *
 * \section1 XML databases
 *
 * \section1 Validity of a unit-system
 *
 * A unit-system loaded from a database is valid if XYZ...
 * An empty unit-system is always valid, an invalid unit-system is considered empty.
 * TBD: isValid() vs hasError(), lastErrorMessage() and ut_set_error_message_handler()
 *
 * \section1 Unit identifiers
 *
 * Within a unit-system, you can map an identifier to a unit and vice versa.
 * If an identifier maps to a unit, then the unit can be retrieved from the unit-system
 * via the identifier. Similarly, if a unit maps to an identifier, then the unit
 * can be printed using the identifier.
 *
 * There a two kinds of identifiers: names and symbols.
 *
 * \section1 Obtaining a unit-system
 *
 * Typically, you would obtain a unit-system of predefined units by reading the
 * default unit database using loadDatabase() with no argument.
 *
 * If this doesn't quite match your needs, then there are alternatives.
 * Together with the typical solution, the means for obtaining a useful unit-system
 * are (in order of increasing complexity):
 * \list
 *  \li Obtain the default unit-system using loadDatabase() with no argument.
 *  \li Copy and customize the unit database XML file and then call loadDatabase()
 *      with the pathname of the customized database to obtain a customized unit-system.
 *  \li Same as either of the above but then adding new units to the unit-system
 *      using addUnit().
 *  \li Same as the above but starting with an empty unit-system, in which case
 *      you will definitely have to start with addBaseUnit() and addDimensionlessUnit().
 * \endlist
 *
 * \section1 Getting a unit from a unit-system
 *
 * Use unitByName() to retrieve a unit by name and unitBySymbol() to retreive a
 * unit by symbol.
 *
 * \section1 Adding new units to a unit-system
 *
 * \note If you use loadDatabase(), then you shouldn't normally need to do this.
 *
 * \section1 Adding new unit-prefixes to a unit-system
 *
 * \note If you use loadDatabase(), then you shouldn't normally need to do this.
 *
 */

/*!
 * \enum UdUnitSystem::DatabaseOrigin
 * This enum type specifies where the unit database has been loaded from:
 * \value NoOrigin
 *        The unit-system has not been loaded from a database.
 * \value UserOrigin
 *        The unit-system has been loaded with a database specified by the user.
 * \value EnvironmentOrigin
 *        The unit-system has been loaded with a database which pathname has been
 *        found in the \e {UDUNITS2_XML_PATH} environment variable.
 * \value SystemOrigin
 *        The unit-system has been loaded with the system default database.
 *        (As determined by the \UU package at compile-time).
 */

/*!
 * Constructs an empty unit-system.
 *
 * An empty unit-system has only one unit defined: the dimensionless unit one.
 */
UdUnitSystem::UdUnitSystem()
{
    m_errorMessage = QString();
    ut_set_status(UT_SUCCESS);
    m_system = ut_new_system();
    if (m_system == nullptr) {
        m_error = ut_get_status();
        //m_errorMessage = ...;
    }
}


/*!
 * \internal
 * Constructs a UdUnitSystem using \UU \a system internal represention.
 */
UdUnitSystem::UdUnitSystem(ut_system *system, ut_status status):
    m_system(system), m_error(status)
{

}

/*!
 * Destroys the unit-system.
 */
UdUnitSystem::~UdUnitSystem()
{
    ut_free_system(m_system);
}

/*!
 * Returns a unit-system corresponding to the XML-formatted unit-database specified by \a pathname.
 * If \a pathname is an empty string (the default), then \UU will try to load
 * a database using the environment variable \e {UDUNITS2_XML_PATH}, if not set,
 * then \UU will load its default database.
 * \sa databaseOrigin(), databasePath()
 */
UdUnitSystem *UdUnitSystem::loadDatabase(const QString &pathname)
{
    ut_set_status(UT_SUCCESS);
    const char *path = nullptr;
    if (!pathname.isEmpty())
        path = pathname.toUtf8().constData();
    ut_system *system = ut_read_xml(path);
    return new UdUnitSystem(system, ut_get_status());
}

/*!
 * Returns the UdUnit to which \a name maps from this unit-system or an invalid
 * UdUnit if no such unit exists ot if this unit-system is invalid.
 * Name comparisons are case-sensitive.
 * \sa UdUnitSystem::isValid(), UdUnit::isValid()
 */
UdUnit UdUnitSystem::unitByName(const QString &name) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_unit_by_name(m_system, name.toLocal8Bit().constData());
    int status = ut_get_status();
    return UdUnit(unit, status);
}
/*!
 * Returns the UdUnit to which \a symbol maps from this unit-system or an invalid
 * UdUnit if no such unit exists or if this unit-system is invalid.
 * Symbol comparisons are case-sensitive.
 * \sa UdUnitSystem::isValid(), UdUnit::isValid()
 */
UdUnit UdUnitSystem::unitBySymbol(const QString &symbol) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_unit_by_symbol(m_system, symbol.toUtf8().constData());
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the dimensionless unit one of this unit-system or an Invalid unit if
 * this unit-system is invalid.
 * \sa UdUnitSystem::isValid(), UdUnit::isValid()
 */
UdUnit UdUnitSystem::dimensionLessUnitOne() const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_dimensionless_unit_one(m_system);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the unit in this unit-system system corresponding to the \a text textual
 * unit representation or an invalid unit if \a text contains a syntax error or
 * an invalid identifier.
 * Unit names and symbols are case-insensitive. \a text must have no leading or
 * trailing whitespace.
 */
UdUnit UdUnitSystem::unitFromString(const QString &text) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_parse(m_system, text.toUtf8().constData(), UT_UTF8);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns true if this unit-system system is valid, false otherwise.
 */
bool UdUnitSystem::isValid()
{
    return m_system != nullptr;
}

/*!
 * TBD
 */
QString UdUnitSystem::errorMessage() const
{
    return m_errorMessage;
}

/*!
 * Creates and adds a new base-unit to this unit-system.
 * This function returns the new unit.
 * If \a name is not empty then this unit can then be retreived using
 * unitByName(), similary if \a symbol is not empty then this unit can then be
 * retreived using unitBySymbol().
 */
UdUnit UdUnitSystem::addBaseUnit(const QString &name, const QString &symbol)
{
    Q_UNUSED(name);
    Q_UNUSED(symbol);
    return UdUnit();
}

/*!
 * Creates and adds a new dimensionless-unit to this unit-system.
 * This function returns the new unit.
 * If \a name is not empty then this unit can then be retreived using
 * unitByName(), similary if \a symbol is not empty then this unit can then be
 * retreived using unitBySymbol().
 */
UdUnit UdUnitSystem::addDimensionlessUnit(const QString &name, const QString &symbol)
{
    Q_UNUSED(name);
    Q_UNUSED(symbol);
    return UdUnit();
}

/*!
 * Adds a new dimensionless-unit to this unit-system.
 * This function returns XXX.
 * If \a name is not empty then this unit can then be retreived using
 * unitByName(), similary if \a symbol is not empty then this unit can then be
 * retreived using unitBySymbol().
 */
bool UdUnitSystem::addUnit(const UdUnit &unit, const QString &name, const QString &symbol)
{
    Q_UNUSED(unit);
    Q_UNUSED(name);
    Q_UNUSED(symbol);
    return false;
}


/*!
 * \internal
 */
ut_status UdUnit::visit_basic(const ut_unit *_unit, void *arg)
{
    Q_UNUSED(_unit);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::BasicUnit;
    return UT_SUCCESS;
}

/*!
 * \internal
 */
ut_status UdUnit::visit_product(const ut_unit *_unit, int count, const ut_unit *const *basicUnits,
                               const int *powers, void *arg)
{
    Q_UNUSED(_unit);
    Q_UNUSED(count);
    Q_UNUSED(basicUnits);
    Q_UNUSED(powers);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::ProductUnit;
    return UT_SUCCESS;
}

/*!
 * \internal
 */
ut_status UdUnit::visit_galilean(const ut_unit *_unit, double scale, const ut_unit *underlyingUnit,
                                double origin, void *arg)
{
    Q_UNUSED(_unit);
    Q_UNUSED(scale);
    Q_UNUSED(underlyingUnit);
    Q_UNUSED(origin);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::GalileanUnit;
    return UT_SUCCESS;
}

/*!
 * \internal
 */
ut_status UdUnit::visit_timestamp(const ut_unit *_unit, const ut_unit *timeUnit,
                                 double origin, void *arg)
{
    Q_UNUSED(_unit);
    Q_UNUSED(timeUnit);
    Q_UNUSED(origin);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::TimestampUnit;
    return UT_SUCCESS;
}

/*!
 * \internal
 */
ut_status UdUnit::visit_logarithmic(const ut_unit *_unit, double base,
                                   const ut_unit *reference, void *arg)
{
    Q_UNUSED(_unit);
    Q_UNUSED(base);
    Q_UNUSED(reference);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::LogarithmicUnit;
    return UT_SUCCESS;
}

ut_visitor UdUnit::m_visitor = {
    &UdUnit::visit_basic,
    &UdUnit::visit_product,
    &UdUnit::visit_galilean,
    &UdUnit::visit_timestamp,
    &UdUnit::visit_logarithmic
};

/*!
 * \class UdUnit
 * \ingroup index
 * \preliminary
 * \brief The UdUnit class represents a unit.
 *
 * TBD
 * \sa UdUnitSystem
 */

/*!
 * \internal
 * Constructs a UdUnit using \UU \a unit internal represention and
 * \UU \a status internal status.
 */
UdUnit::UdUnit(ut_unit *unit, int status):
    m_unit(unit), m_errorStatus(status),
    m_type(NullUnit)
{
    ut_accept_visitor(m_unit, &m_visitor, (void *)(this));
}

/*!
 * Constructs an invalid unit.
 */
UdUnit::UdUnit():
    m_unit(nullptr),
    m_type(NullUnit)
{

}

/*!
 * Constructs a copy of \a other.
 */
UdUnit::UdUnit(const UdUnit &other):
    m_errorStatus(other.m_errorStatus),
    m_type(NullUnit)
{
    m_unit = ut_clone(other.m_unit);
}

/*!
 * Destroys the unit.
 */
UdUnit::~UdUnit()
{
    ut_free(m_unit);
}

/*!
 * Returns true if this unit is valid, false otherwise.
 */
bool UdUnit::isValid() const
{
    return m_unit != nullptr;
}

/*!
 * Returns the unit-system this unit belongs to or an invalid unit-system if this
 * unit doesn't belong to any unit-system or if this unit is not valid (FIXME).
 */
UdUnitSystem UdUnit::system()
{
    ut_set_status(UT_SUCCESS);
    ut_system *system = ut_get_system(m_unit);
    return UdUnitSystem(system, ut_get_status());
}

/*!
 * TBD
 */
UdUnit::UnitType UdUnit::type() const
{
    return m_type;
}

/*!
 * Returns an UTF-8 textual representation of this unit's name or an empty string
 * if this unit is not a named unit or if this unit is invalid (TBD).
 */
QString UdUnit::name() const
{
    return QString(ut_get_name(m_unit, UT_UTF8));
}

/*!
 * Returns an UTF-8 textual representation of this unit's symbol or an empty string
 * if this unit doesn't have one or if this unit is invalid (TBD).
 */
QString UdUnit::symbol() const
{
    return QString(ut_get_symbol(m_unit, UT_UTF8));
}

/*!
 * Return an UTF-8 textual representation of this unit according to \a form and \a option.
 * \sa UdUnit::FormatForm, UdUnit::FormatOption
 */
QString UdUnit::format(FormatForm form, FormatOption option) const
{
    static const int size = 256;
    char buffer[size + 1];
    int flags = UT_UTF8;
    if (form == DefinitionForm)
        flags |= UT_DEFINITION;
    if (option == UseUnitName)
        flags |= UT_NAMES;
    int nBytes = ut_format(m_unit, buffer, 256, flags);
    if (nBytes < 0 || nBytes > size)
        return QString();
    else
        return QString::fromUtf8(buffer, nBytes);
}

/*!
 * Returns true if this unit is dimensionless, false otherwise.
 * An invalid unit is considered dimensionfull.
 */
bool UdUnit::isDimensionless() const
{
    ut_set_status(UT_SUCCESS);
    int result = ut_is_dimensionless(m_unit);
    //int status = ut_get_status();
    return result != 0;
}

/*!
 * Returns a unit equivalent to this unit scaled by \a factor.
 * For example:
 * \code
 * UdUnit meter = ...
 * UdUnit kilometer = meter.scaledBy(1000.0);
 * \endcode
 */
UdUnit UdUnit::scaledBy(qreal factor) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_scale(factor, m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns a unit equivalent to this unit relative to the origin defined by \a offset.
 * For example:
 * \code
 * UdUnit kelvin = ...
 * UdUnit celcius = kelvin.offsetBy(273.15);
 * \endcode
 */
UdUnit UdUnit::offsetBy(qreal offset) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset(m_unit, offset);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * FIXME: get rid of time
 */
UdUnit UdUnit::offsetByTime(qreal origin) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset_by_time(m_unit, origin);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the inverse of this unit. This convenience function is equal to raisedBy(-1).
 */
UdUnit UdUnit::inverted() const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_invert(m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the unit equal to this unit raised by \a power.
 */
UdUnit UdUnit::raisedBy(int power) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_raise(m_unit, power);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the unit equal to the \a root 'th root of this unit.
 */
UdUnit UdUnit::rootedBy(int root) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_root(m_unit, root);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the unit corresponding to the logarithmic \a base base
 * using this unit as a reference level.
 * For example, the following creates a decibel unit with a one milliwatt reference level:
 * \code
 * UdUnit milliWatt = ...
 * UdUnit decibel_1_mW = milliWatt.toLogarithmic(10.0).scaledBy(0.1);
 * \endcode
 */
UdUnit UdUnit::toLogarithmic(qreal base) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_log(base, m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * TBD
 */
UdUnit operator +(const UdUnit &lhs, qreal rhs)
{
    return lhs.offsetBy(rhs);
}

/*!
 * TBD
 */
UdUnit operator +(qreal lhs, const UdUnit &rhs)
{
    return rhs.offsetBy(lhs);
}

/*!
 * TBD
 */
UdUnit operator -(const UdUnit &lhs, qreal rhs)
{
    return lhs.offsetBy(-rhs);
}

/*!
 * TBD
 */
UdUnit operator -(qreal lhs, const UdUnit &rhs)
{
    return rhs.offsetBy(-lhs);
}

/*!
 * TBD
 */
UdUnit operator *(const UdUnit &lhs, qreal rhs)
{
    return lhs.scaledBy(rhs);
}

/*!
 * TBD
 */
UdUnit operator *(qreal lhs, const UdUnit &rhs)
{
    return rhs.scaledBy(lhs);
}

/*!
 * TBD
 */
UdUnit operator *(const UdUnit &lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_multiply(lhs.m_unit, rhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * TBD
 */
UdUnit operator /(const UdUnit &lhs, qreal rhs)
{
    return lhs.scaledBy(1/rhs);
}

/*!
 * TBD
 */
UdUnit operator /(qreal lhs, const UdUnit &rhs)
{
    return rhs.inverted().scaledBy(lhs);
}

/*!
 * TBD
 */
UdUnit operator /(const UdUnit &lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_divide(lhs.m_unit, rhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * TBD
 */
bool operator ==(const UdUnit &lhs, const UdUnit &rhs)
{
    return ut_compare(lhs.m_unit, rhs.m_unit) == 0;
}

/*!
 * \class UdUnitConverter
 * \preliminary
 * \ingroup index
 * \brief The UdUnitConverter class converts values between 2 units.
 *
 * TBD
 * \sa UdUnitSystem
 */

/*!
 * Constructs a converter from \a from unit to \a to unit.
 */
UdUnitConverter::UdUnitConverter(const UdUnit &from, const UdUnit &to):
    m_from(from), m_to(to), m_converter(nullptr)
{
    ut_set_status(UT_SUCCESS);
    m_converter = ut_get_converter(m_from.m_unit, m_to.m_unit);
    //m_status = ut_get_status();
}

/*!
 * Destroys the converter;
 */
UdUnitConverter::~UdUnitConverter()
{
    cv_free(m_converter);
}

/*!
 * Return true if this converter is valid, false otherwise.
 * A converter is invalid if it's from and to units are not convertible.
 */
bool UdUnitConverter::isValid() const
{
    return m_converter != nullptr;
}

/*!
 * Returns \a value (which is expressed in the converter's from unit) converted
 * to this converter's to unit. If the converter is invalid, the behaviour is undefined.
 */
qreal UdUnitConverter::convert(qreal value)
{
    return cv_convert_double(m_converter, value);
}

/*!
 * Returns \a values (which is expressed in the converter's from unit) converted
 * to this converter's to unit. If the converter is invalid, the behaviour is undefined.
 */
QVector<qreal> UdUnitConverter::convert(const QVector<qreal> values)
{
    QVector<qreal> result(values.size());
    cv_convert_doubles(m_converter, values.data(), values.size(), result.data());
    return result;
}

/*!
 * Converts in-place \a values (which is expressed in the converter's from unit)
 * to this converter's to unit. If the converter is invalid, the behaviour is undefined.
 * This function returns a reference to \a values as a convenience.
 */
QVector<qreal> &UdUnitConverter::convert(QVector<qreal> &values)
{
    cv_convert_doubles(m_converter, values.data(), values.size(), values.data());
    return values;
}
