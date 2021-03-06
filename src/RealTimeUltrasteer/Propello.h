#pragma once

#include "ui_propello.h"
#include "stdafx.h"
#include "RobotControl.h"
#include "Segment.h"

#define prmColor_Mode "color mode"
#define prmColor_PRP "color-prp"
#define prmColor_WF "color-wf"

class porta;
class RobotControl;

class Propello : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    Propello(QWidget* parent = 0);
    virtual ~Propello();
	void DisplayErrorBox(int i);

private:
    bool loadMode(int mode, int rfmode);
    bool loadMasterPreset();
    void setupControls();
    void refreshParams();
    void stepMotor(bool fwd);
    QString getParamStringValue(QString);
	bool startScanningNeedle();
	void enableRobotControls(bool);
	void setDopplerSettings(f64 prf, f64 wf, s32 colorMode);

private:
    QString m_firmwarePath;
    QString m_settingsPath;
    int m_cineSize;
    int m_motorstart;
    double m_motorpos;
    porta* m_porta;
    probeInfo m_probeInfo;
    imagingMode mode;
	bool m_needleScanning;
	bool m_closedLoopSteering;
	bool m_rollInit;
	bool m_insInit;
	RobotControl m_robotcontrol;

	Nf::NeedleSegmenter *m_ns;

	QMutex m_mutex;

signals:
    void showInfo(int);

private slots:
    void initHardware();
    void runImage();
    void stopImage();
    void showSystemID();
    void getFirmwarePath();
    void getSettingsPath();
    void setCineSize();
    void onDetect();
    void onProbeAttributes();
    void onCaptureChange(int);
    void onDataSelection(int);
    void onShowInfo(int);
    void onHomeMotor();
    void incParam();
    void decParam();
    void onApply();
    void cineScroll(int);
    void onStoreVolume();
	void scanNeedle();
	void InitializeInsertion();
	void InitializeRoll();
	void applyControl();
	void Insert2mm();
	void Retract2mm();
	void Insert30rpm();
	void Retract30rpm();
	void StopInsertion();
	void SetMaxPosVelocity();
	void SetMaxNegVelocity();
	void StopRoll();
	void RotPosInc();
	void RotNegInc();
	void homeRoll();
	void homeInsertion();
	void setTargetX(int);
	void setTargetY(int);
	void setTargetZ(int);
	void onTick(int framenum);
};
