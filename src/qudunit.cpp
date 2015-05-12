#include "qudunit.h"

#include <QDebug>


/*!
 * \class UdUnitSystem
 * \preliminary
 * \brief The UdUnitSystem class represent a unit-system.
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
 * TBD: XML databases
 */

UdUnitSystem::UdUnitSystem()
{
    m_system = ut_read_xml(nullptr);
    if (m_system == nullptr)
        m_error = ut_get_status();
    else
        m_error = UT_SUCCESS;
}

UdUnitSystem::~UdUnitSystem()
{
    ut_free_system(m_system);
}

/*!
 * Returns the UdUnit to which \a name maps from this unit-system or an invalid
 * UdUnit if no such unit exists ot if this unit-system is invalid.
 * Name comparisons are case-sensitive.
 * \sa UdUnitSystem::isValid() and UdUnit::isValid()
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
 * \sa UdUnitSystem::isValid() and UdUnit::isValid()
 */
UdUnit UdUnitSystem::unitBySymbol(const QString &name) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_unit_by_symbol(m_system, name.toUtf8().constData());
    int status = ut_get_status();
    return UdUnit(unit, status);
}

/*!
 * Returns the dimensionless unit one of this unit-system or an Invalid unit if
 * this unit-system is invalid.
 * \sa UdUnitSystem::isValid() and UdUnit::isValid()
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
 * \internal
 * Constructs a UdUnitSystem using libudunits2 \a system internal represention.
 */
UdUnitSystem::UdUnitSystem(ut_system *system):
    m_system(system)
{

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
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::LogarithmicUnit;
    return UT_SUCCESS;
}

/*!
 * \internal
 */
ut_visitor UdUnit::m_visitor = {
    &UdUnit::visit_basic,
    &UdUnit::visit_product,
    &UdUnit::visit_galilean,
    &UdUnit::visit_timestamp,
    &UdUnit::visit_logarithmic
};

/*!
 * \class UdUnit
 * \preliminary
 * \brief The UdUnit class represents a unit.
 * TBD
 * \sa UdUnitSystem
 */

/*!
 * \internal
 * Constructs a UdUnit using libudunits2 \a unit internal represention and
 * libudunits2 \a status internal status.
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
    ut_system *system = ut_get_system(m_unit);
    return UdUnitSystem(system);
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
 * \sa UdUnit::FormatForm and UdUnit::FormatOption
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
    int status = ut_get_status();
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
 * Returns the unit equal to this unit raised by \power.
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
 * \brief This class allows to do convertion between 2 units.
 * TBD
 * \sa UdUnitSystem
 */

/*!
 * Constructs a converter from \a from unit to \to unit.
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
