#include "qudunit.h"

#include <QDebug>

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

UdUnit UdUnitSystem::unitByName(const QString &name) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_unit_by_name(m_system, name.toLocal8Bit().constData());
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit UdUnitSystem::unitBySymbol(const QString &name) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_unit_by_symbol(m_system, name.toUtf8().constData());
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit UdUnitSystem::dimensionLessUnitOne() const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_get_dimensionless_unit_one(m_system);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit UdUnitSystem::unitFromString(const QString &string) const
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_parse(m_system, string.toUtf8().constData(), UT_UTF8);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

bool UdUnitSystem::isValid()
{
    return m_system != nullptr;
}

QString UdUnitSystem::errorMessage() const
{
    return m_errorMessage;
}

ut_status UdUnit::visit_basic(const ut_unit *_unit, void *arg)
{
    Q_UNUSED(_unit);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::BasicUnit;
    return UT_SUCCESS;
}

ut_status UdUnit::visit_product(const ut_unit *_unit, int count, const ut_unit *const *basicUnits,
                               const int *powers, void *arg)
{
    Q_UNUSED(_unit);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::ProductUnit;
    return UT_SUCCESS;
}

ut_status UdUnit::visit_galilean(const ut_unit *_unit, double scale, const ut_unit *underlyingUnit,
                                double origin, void *arg)
{
    Q_UNUSED(_unit);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::GalileanUnit;
    return UT_SUCCESS;
}

ut_status UdUnit::visit_timestamp(const ut_unit *_unit, const ut_unit *timeUnit,
                                 double origin, void *arg)
{
    Q_UNUSED(_unit);
    UdUnit *unit = (UdUnit*)(arg);
    unit->m_type = UdUnit::TimeStampUnit;
    return UT_SUCCESS;
}

ut_status UdUnit::visit_logarithmic(const ut_unit *_unit, double base,
                                   const ut_unit *reference, void *arg)
{
    Q_UNUSED(_unit);
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

UdUnit::UdUnit(ut_unit *unit, int status):
    m_unit(unit), m_errorStatus(status)
{
    ut_accept_visitor(m_unit, &m_visitor, (void *)(this));
}

UdUnit::UdUnit()
{

}

UdUnit::UdUnit(const UdUnit &other)
{
    m_unit = ut_clone(other.m_unit);
    ut_accept_visitor(m_unit, &m_visitor, (void *)(this));
}

UdUnit::~UdUnit()
{
    ut_free(m_unit);
}

bool UdUnit::isValid() const
{
    return m_unit != nullptr;
}

UdUnit::Type UdUnit::type() const
{
    return m_type;
}

QString UdUnit::name() const
{
    return QString(ut_get_name(m_unit, UT_UTF8));
}

QString UdUnit::symbol() const
{
    return QString(ut_get_symbol(m_unit, UT_UTF8));
}

QString UdUnit::toString() const
{
    static const int size = 256;
    char buffer[size + 1];
    int nBytes = ut_format(m_unit, buffer, 256, UT_UTF8 | UT_NAMES);
    if (nBytes < 0 || nBytes > size)
        return QString();
    else
        return QString::fromUtf8(buffer, nBytes);
}

UdUnit operator +(const UdUnit &lhs, qreal rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset(lhs.m_unit, rhs);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator +(qreal lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset(rhs.m_unit, lhs);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator -(const UdUnit &lhs, qreal rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset(lhs.m_unit, -rhs);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator -(qreal lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_offset(rhs.m_unit, -lhs);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

bool operator ==(const UdUnit &lhs, const UdUnit &rhs)
{
    return ut_compare(lhs.m_unit, rhs.m_unit) == 0;
}

UdUnit operator *(const UdUnit &lhs, qreal rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_scale(rhs, lhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator *(qreal lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_scale(lhs, rhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator *(const UdUnit &lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_multiply(lhs.m_unit, rhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator /(const UdUnit &lhs, qreal rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_scale(1.0/rhs, lhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}

UdUnit operator /(qreal lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_invert(rhs.m_unit);
    int status = ut_get_status();
    return lhs * UdUnit(unit, status);
}

UdUnit operator /(const UdUnit &lhs, const UdUnit &rhs)
{
    ut_set_status(UT_SUCCESS);
    ut_unit *unit = ut_divide(lhs.m_unit, rhs.m_unit);
    int status = ut_get_status();
    return UdUnit(unit, status);
}



UdUnitConverter::UdUnitConverter(const UdUnit &from, const UdUnit &to):
    m_from(from), m_to(to)
{
    m_converter = ut_get_converter(m_from.m_unit, m_to.m_unit);
}

UdUnitConverter::~UdUnitConverter()
{
    cv_free(m_converter);
}

bool UdUnitConverter::canConvert() const
{
    return ut_are_convertible(m_from.m_unit, m_to.m_unit) != 0;
}

qreal UdUnitConverter::convert(qreal value)
{
    Q_ASSERT(m_converter != nullptr);
    return cv_convert_double(m_converter, value);
}
