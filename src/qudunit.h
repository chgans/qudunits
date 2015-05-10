#ifndef QUDUNIT_H
#define QUDUNIT_H

#include "qudunit_global.h"

#include <QString>
#include <QStringList>
#include <QVector>

#include <udunits2.h>

class UdUnitSystem;
class UdUnitPrefix;
class UdUnit;
class UdUnitConverter;

class QUDUNITSHARED_EXPORT UdUnit {
public:
    enum Type {
        NullUnit = 0,
        BasicUnit,
        ProductUnit,
        GalileanUnit,
        TimeStampUnit,
        LogarithmicUnit
    };

    UdUnit();
    UdUnit(const UdUnit &other);
    ~UdUnit();

    bool isValid() const;

    // isBase, is DimensionLess
    Type type() const;
    QString name() const;
    QString symbol() const;
    QString toString() const;
    UdUnitSystem system();

    // operations
    // scale, offset, invert, raise, root, log

    // Basic units: base unit or non-dimensional named unit
    // Product units: QMap<BasicUnit, Power>
    // Galilean units: underlyingUInit and scale or origin
    //
    // Log units: base and reference


    friend bool operator ==(const UdUnit &lhs, const UdUnit &rhs);
    inline friend bool operator !=(const UdUnit &lhs, const UdUnit &rhs)
    { return !(lhs == rhs); }

    friend UdUnit operator *(const UdUnit &lhs, const UdUnit &rhs);
    friend UdUnit operator *(const UdUnit &lhs, qreal rhs);
    friend UdUnit operator *(qreal lhs, const UdUnit &rhs);
    friend UdUnit operator /(const UdUnit &lhs, const UdUnit &rhs);
    friend UdUnit operator /(const UdUnit &lhs, qreal rhs);
    friend UdUnit operator /(qreal lhs, const UdUnit &rhs);
    friend UdUnit operator +(const UdUnit &lhs, qreal rhs);
    friend UdUnit operator +(qreal lhs, const UdUnit &rhs);
    friend UdUnit operator -(const UdUnit &lhs, qreal rhs);
    friend UdUnit operator -(qreal lhs, const UdUnit &rhs);

private:
    friend class UdUnitSystem;
    friend class UdUnitConverter;
    UdUnit(ut_unit *unit, int status);

    static ut_visitor m_visitor;
    static ut_status visit_basic(const ut_unit *_unit, void *arg);
    static ut_status visit_product(const ut_unit *_unit, int count, const ut_unit *const *basicUnits,
                                   const int *powers, void *arg);
    static ut_status visit_galilean(const ut_unit *_unit, double scale, const ut_unit *underlyingUnit,
                                    double origin, void *arg);
    static ut_status visit_timestamp(const ut_unit *_unit, const ut_unit *timeUnit,
                                     double origin, void *arg);
    static ut_status visit_logarithmic(const ut_unit *_unit, double base,
                                       const ut_unit *reference, void *arg);
    ut_unit *m_unit;
    int m_errorStatus;
    Type m_type;
};

class QUDUNITSHARED_EXPORT UdUnitConverter {

public:
    UdUnitConverter(const UdUnit &from, const UdUnit &to);
    ~UdUnitConverter();

    UdUnit fromUnit() const;
    UdUnit toUnit() const;

    bool isValid() const;
    qreal convert(qreal value);
    QVector<qreal> convert(const QVector<qreal> values);

private:
    UdUnit m_from;
    UdUnit m_to;
    cv_converter *m_converter = nullptr;
};

class QUDUNITSHARED_EXPORT UdUnitSystem
{
public:
    UdUnitSystem();
    ~UdUnitSystem();

    UdUnit unitByName(const QString &name) const;
    UdUnit unitBySymbol(const QString &name) const;
    UdUnit dimensionLessUnitOne() const;
    UdUnit unitFromString(const QString &string) const;

    bool isValid();
    QString errorMessage() const;

private:
    ut_system *m_system;
    int m_error;
    QString m_errorMessage;
};

#endif // QUDUNIT_H
