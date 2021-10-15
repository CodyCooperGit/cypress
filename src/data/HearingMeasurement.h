#ifndef HEARINGMEASUREMENT_H
#define HEARINGMEASUREMENT_H

#include "MeasurementBase.h"

/*!
 * \class HearingMeasurement
 * \brief A HearingMeasurement class
 *
 * \sa MeasurementBase
 */

class HearingMeasurement :  public MeasurementBase
{   
public:

    void fromCode(const int &, const QString &);

    QString toString() const;

    bool isValid() const;    

    static QMap<QString,QString> initCodeLookup();
    static QMap<QString,QString> initOutcomeLookup();
    static QMap<int,QString> initFrequencyLookup();

private:
    static QMap<QString,QString> codeLookup;// = HearingMeasurement::initCodeLookup();
    static QMap<QString,QString> outcomeLookup;// = HearingMeasurement::initOutcomeLookup();
    static QMap<int,QString> frequencyLookup;// = HearingMeasurement::initFrequencyLookup();
};

Q_DECLARE_METATYPE(HearingMeasurement);

#endif // HEARINGMEASUREMENT_H
