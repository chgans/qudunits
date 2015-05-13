#ifndef QUDUNIT_H
#define QUDUNIT_H

#include "qudunit_global.h"

#include <QMetaType>
#include <QString>
#include <QVector>
#include <QMap>

#include <udunits2.h>

class UdUnitSystem;
class UdUnitPrefix;
class UdUnit;
class UdUnitConverter;

class QUDUNITSHARED_EXPORT UdUnit {
public:
    enum UnitType {
        NullUnit = 0,
        BasicUnit,
        ProductUnit,
        GalileanUnit,
        TimestampUnit,
        LogarithmicUnit
    };

    enum FormatOption {
        UseUnitName = 0,
        UseUnitSymbol
    };

    enum FormatForm {
        ShortForm = 0,
        DefinitionForm
    };

    UdUnit();
    UdUnit(const UdUnit &other);
    ~UdUnit();

    bool isValid() const;
    UdUnitSystem system();
    UnitType type() const;
    QString name() const;
    QString symbol() const;

    // TODO: rename to format, add format option name/symbol and definition
    QString format(FormatForm form = ShortForm, FormatOption option = UseUnitSymbol) const;

    // Basic-unit: A basic-unit is a base unit like “meter” or a non-dimensional but named unit like “radian”.
    inline bool isBasic() const { return type() == BasicUnit; }
    bool isDimensionless() const;

    // Product-unit: unit, non-zero power
    inline bool isProduct() const { return type() == ProductUnit; }
    QMap<UdUnit, int> productPoweredUnits() const;

    // Galilean-unit:
    inline bool isGalilean() const { return type() == GalileanUnit; }
    UdUnit underlyingUnit() const; // referenceUnit()
    qreal galileanScaleFactor() const; // non-unity scale factor
    qreal galileanOrigin() const;     // origin()

    // Log-unit: base and reference
    inline bool isLogarithmic() const { return type() == LogarithmicUnit; }
    qreal logBase() const;
    UdUnit referenceUnit() const;// referenceUnit()

    // Timestamp-unit:
    inline bool isTimestamp() const { return type() == TimestampUnit; }
    UdUnit timeUnit() const; // referenceUnit()
    qreal timeOrigin() const; // origin()

    // operations for equality, comparison
    friend bool operator ==(const UdUnit &lhs, const UdUnit &rhs);
    inline friend bool operator !=(const UdUnit &lhs, const UdUnit &rhs)
    { return !(lhs == rhs); }

    // operations for scale, offset, invert, raise, root, log
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

    UdUnit scaledBy(qreal factor) const;
    UdUnit offsetBy(qreal offset) const;
    UdUnit offsetByTime(qreal origin) const;
    UdUnit inverted() const;
    UdUnit raisedBy(int power) const;
    UdUnit rootedBy(int root) const;
    UdUnit toLogarithmic(qreal base) const;

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
    UnitType m_type;
};

Q_DECLARE_METATYPE(UdUnit::UnitType);


class QUDUNITSHARED_EXPORT UdUnitConverter {

public:
    UdUnitConverter(const UdUnit &from, const UdUnit &to);
    ~UdUnitConverter();

    UdUnit fromUnit() const;
    UdUnit toUnit() const;

    bool isValid() const;
    qreal convert(qreal value);
    QVector<qreal> convert(const QVector<qreal> values);
    QVector<qreal> &convert(QVector<qreal> &values);

    // TODO:
    static bool canConvert(const UdUnit &from, const UdUnit &to);

private:
    UdUnit m_from;
    UdUnit m_to;
    cv_converter *m_converter;
};

// TODO: Allow to specify XML path
//       either at construct time or maybe as a property
// TODO: Should we parse the files to offer enumeration service?
class QUDUNITSHARED_EXPORT UdUnitSystem
{
public:
    enum DatabaseOrigin {
        NoOrigin,
        UserOrigin,
        SystemOrigin,
        EnvironmentOrigin
    };

    UdUnitSystem();
    ~UdUnitSystem();

    static UdUnitSystem *loadDatabase(const QString &pathname = QString());

    UdUnit unitByName(const QString &name) const;
    UdUnit unitBySymbol(const QString &symbol) const;
    UdUnit dimensionLessUnitOne() const;
    UdUnit unitFromString(const QString &text) const;

    bool isValid();
    QString errorMessage() const;

    QString databasePath() const;
    DatabaseOrigin databaseOrigin() const;

    UdUnit addBaseUnit(const QString &name, const QString &symbol);
    UdUnit addDimensionlessUnit(const QString &name, const QString &symbol);
    bool addUnit(const UdUnit &unit, const QString &name, const QString &symbol);
    bool addPrefix(const QString &name, const QString &symbol);

private:
    friend class UdUnit;
    UdUnitSystem(ut_system *system, ut_status status);
    UdUnitSystem(const UdUnitSystem &other);
    ut_system *m_system;
    int m_error;
    QString m_errorMessage;
};



#endif // QUDUNIT_H
