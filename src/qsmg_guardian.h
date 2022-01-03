#pragma once

#include <QVariantHash>
#include "./qsmg_object.h"

namespace QSMG {

//!
//! \brief The Object class
//!
//! basic object class
class Q_SMG_EXPORT Guardian: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash settings READ settings WRITE setSettings NOTIFY settingsChanged)
public:

    //!
    //! \brief Guardian
    //! \param parent
    //!
    explicit Guardian(QObject*parent=nullptr);

    //!
    //!
    ~Guardian();

    //!
    //! \brief settings
    //! \return
    //!
    const QVariantHash &settings() const;

    //!
    //! \brief setSettings
    //! \param v
    //! \return
    //!
    Guardian&setSettings(const QVariantHash &v);

    //!
    //! \brief start
    //! \return
    //!
    Guardian&start();

    //!
    //! \brief start
    //! \param program
    //! \param arguments
    //! \return
    //!
    Guardian&start(const QByteArray&program, const QByteArray&arguments);

    //!
    //! \brief isRunning
    //! \return
    //!
    bool isRunning();

    //!
    //! \brief waitFinished
    //! \param time
    //! \return
    //!
    Guardian&waitFinished(unsigned long time=0);

    //!
    //! \brief terminate
    //! \param time
    //! \return
    //!
    Guardian&terminate();

signals:
    //!
    //! \brief settingsChanged
    //!
    void settingsChanged();

    //!
    //! \brief processStart
    //!
    void processStart();

    //!
    //! \brief processStats
    //! \param stats
    //!
    void processStats(const QVariantHash stats);

    //!
    //! \brief processFinished
    //!
    void processFinished();

    //!
    //! \brief processStandardOutput
    //! \param output
    //!
    void processStandardOutput(const QByteArray output);

    //!
    //! \brief processStandardError
    //! \param output
    //!
    void processStandardError(const QByteArray output);
private:
    void*p=nullptr;
};

}

