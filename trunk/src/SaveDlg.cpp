#include "stdafx.h"
#include "SaveDlg.h"
#include "Propello.h"

struct propelloHeader
{
    int type;
    int volumes;
    int fpv;
    int w;
    int h;
    int ss;
    int degPerFr;
};

// taken from uData.h
enum uData
{
    udtScreen = 0x00000001,
    udtBPre = 0x00000002,
    udtBPost = 0x00000004,
    udtBPost32 = 0x00000008,
    udtRF = 0x00000010,
    udtMPre = 0x00000020,
    udtMPost = 0x00000040,
    udtPWRF = 0x00000080,
    udtPWSpectrum = 0x00000100,
    udtColorRF = 0x00000200,
    udtColorPost = 0x00000400,
    udtColorVelocityVariance = 0x00000800,
    udtElastoCombined = 0x00002000,
    udtElastoOverlay = 0x00004000,
    udtElastoPre = 0x00008000,
    udtECG = 0x00010000
};

// taken from uFileHeader.h
class uFileHeader
{
public:
    /// data type - data types can also be determined by file extensions
    int type;
    /// number of frames in file
    int frames;
    /// width - number of vectors for raw data, image width for processed data
    int w;
    /// height - number of samples for raw data, image height for processed data
    int h;
    /// data sample size in bits
    int ss;
    /// roi - upper left (x)
    int ulx;
    /// roi - upper left (y)
    int uly;
    /// roi - upper right (x)
    int urx;
    /// roi - upper right (y)
    int ury;
    /// roi - bottom right (x)
    int brx;
    /// roi - bottom right (y)
    int bry;
    /// roi - bottom left (x)
    int blx;
    /// roi - bottom left (y)
    int bly;
    /// probe identifier - additional probe information can be found using this id
    int probe;
    /// transmit frequency
    int txf;
    /// sampling frequency
    int sf;
    /// data rate - frame rate or pulse repetition period in Doppler modes
    int dr;
    /// line density - can be used to calculate element spacing if pitch and native # elements is known
    int ld;
    /// extra information - ensemble for color RF
    int extra;
};

SaveDlg::SaveDlg(storageType type)
{
    setupUi(this);

    m_storageType = type;
    m_prescan = false;
    m_postscan = false;
    m_rf = false;
    m_colorvv = false;
    m_colorpost = false;

    wFilePath->setText("D:\\");
    int mode = portaGetCurrentMode();

    //number of volumes
    QTableWidgetItem* item = wSaveOptions->item(0, 8);
    if (item)
    {
        item->setText(QString("%1").arg(portaGetDisplayFrameCount(0) / portaGetParam(prmMotorFrames)));
    }

    //disable the options depending on the imaging mode and storage type
    if (mode != ColourMode)
    {
        disableEdit(wSaveOptions->item(0, 4));
        disableEdit(wSaveOptions->item(0, 5));
    }
    if (m_storageType == storeAutoVolume)
    {
        disableEdit(wSaveOptions->item(0, 7));
    }

    m_fileNumber = 0;
}

SaveDlg::~SaveDlg()
{
}

void SaveDlg::onChangeDirectory()
{
    wFilePath->clear();
    m_filePath = QFileDialog::getExistingDirectory(this, tr("Create volume file"), m_filePath, QFileDialog::ShowDirsOnly);
    wFilePath->setText(m_filePath);
}

void SaveDlg::onSave()
{
    //get the save options
    bool status;
    updateSaveOptions();

    m_fileNumber++;

    // write to file
    if (m_storageType == storeAutoVolume)
    {
        status = saveAutoVolume();
    }
    else if (m_storageType == storeManualVolume && (wSaveOptions->item(0, 7)->checkState() != Qt::Checked))
    {
        status = saveManualVolume();
    }
    else if (m_storageType == storeManualVolume && (wSaveOptions->item(0, 7)->checkState() == Qt::Checked))
    {
        status = saveSingleFrame();
    }

    QMessageBox msgBox;
    if (status)
    {
        msgBox.setText("File(s) saved sucessfully!");
    }
    else
    {
        msgBox.setText("Error saving to file!");
    }

    msgBox.exec();
}

bool SaveDlg::saveAutoVolume()
{
    int i, frames = portaGetDisplayFrameCount(0);
    int fpv = portaGetParam(prmMotorFrames), steps = portaGetParam(prmMotorSteps);
    int volumes = frames / fpv, szFrm;
    probeInfo nfo;
    propelloHeader hdr;
    unsigned char* buffer;
    QString defaultFile;
    FILE* fp = 0;

    portaGetProbeInfo(nfo);
    double degPerStep = (double)nfo.motorFov / (double)nfo.motorSteps;

    hdr.degPerFr = (int)(steps * degPerStep);
    hdr.fpv = fpv;
    hdr.volumes = volumes;

    if (m_prescan)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_auto_prescan.vol").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }

        portaGetPrescanBDimensions(0, hdr.w, hdr.h);
        hdr.type = 0; //BPre;
        hdr.ss = 8;
        szFrm = hdr.w * hdr.h * hdr.ss / 8;
        buffer = new unsigned char[szFrm];

        fwrite(&hdr, sizeof(hdr), 1, fp);

        for (i = 0; i < (volumes * fpv); i++)
        {
            portaGetBwImagePrescan(0, buffer);
            fwrite(buffer, szFrm, 1, fp);
        }

        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }

    if (m_postscan)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_auto_postscan.vol").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }

        portaGetDisplayDimensions(0, hdr.w, hdr.h);
        hdr.type = 1; //BPost;
        hdr.ss = 8;
        szFrm = hdr.w * hdr.h * hdr.ss / 8;
        buffer = new unsigned char[szFrm];

        fwrite(&hdr, sizeof(hdr), 1, fp);

        for (i = 0; i < (volumes * fpv); i++)
        {
            portaProcessCineImage(0, i);
            portaGetBwImage(0, buffer, false);
            fwrite(buffer, szFrm, 1, fp);
        }

        delete[] buffer;
        if (fp)
        {
            fclose(fp);
        }
    }

    if (m_rf)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_auto_rf.vol").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }

        hdr.type = 2; //RF
        hdr.w = portaGetParam(prmRfNumLines);
        hdr.h = portaGetParam(prmRfNumSamples) >> portaGetParam("b-decimation");
        hdr.ss = 16;
        szFrm = (hdr.w * hdr.h * (hdr.ss / 8));

        fwrite(&hdr, sizeof(hdr), 1, fp);

        for (i = 0; i < (volumes * fpv); i++)
        {
            fwrite(portaGetFrameAddress(0, i), szFrm, 1, fp);
        }

        if (fp)
        {
            fclose(fp);
        }
    }

    if (m_colorvv)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_auto_colorvv.vol").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }

        portaGetDisplayDimensions(0, hdr.w, hdr.h);
        szFrm = hdr.w * hdr.h * 2;
        buffer = new unsigned char[szFrm];
        hdr.type = 3;
        hdr.ss = 16;

        fwrite(&hdr, sizeof(hdr), 1, fp);

        for (i = 0; i < (volumes * fpv); i++)
        {
            portaProcessCineImage(0, i);
            portaGetColorVV(0, buffer);
            fwrite(buffer, szFrm, 1, fp);
        }

        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }

    if (m_colorpost)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_auto_colorpost.vol").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }

        portaGetDisplayDimensions(0, hdr.w, hdr.h);
        hdr.type = 4;
        hdr.ss = 32;
        szFrm = hdr.w * hdr.h * hdr.ss / 8;
        buffer = new unsigned char[szFrm];

        fwrite(&hdr, sizeof(hdr), 1, fp);

        for (i = 0; i < (volumes * fpv); i++)
        {
            portaProcessCineImage(0, i);
            portaGetColorImage(0, buffer);
            fwrite(buffer, szFrm, 1, fp);
        }
        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }

    return true;
}

bool SaveDlg::saveManualVolume()
{
    probeInfo nfo;
    portaGetProbeInfo(nfo);
    FILE* fp = 0;
    propelloHeader hdr;

    int i, szFrm;
    int fpv = portaGetParam(prmMotorFrames), steps = portaGetParam(prmMotorSteps);
    double degPerStep = (double)nfo.motorFov / (double)nfo.motorSteps;
    double fov = fpv * degPerStep * (double)steps;
    int slptime = 50;

    unsigned char* buffer;
    QString defaultFile;

    hdr.degPerFr = (int)(steps * degPerStep);
    hdr.fpv = fpv;
    hdr.volumes = 1;

    // calculate the start of the volume, and move the motor there
    double pos = ((nfo.motorFov / 1000.0) / 2) - ((fov / 1000.0) / 2);
    portaGoToPosition(pos);

    defaultFile = m_filePath;

    if (m_prescan)
    {
        defaultFile.append(QString("\\%2_manual_prescan.vol").arg(m_fileNumber));
        portaGetPrescanBDimensions(0, hdr.w, hdr.h);
        hdr.type = 0; //udtBPre;
        hdr.ss = 8;
        szFrm = hdr.w * hdr.h;
        buffer = new unsigned char[szFrm];
    }
    else if (m_postscan)
    {
        defaultFile.append(QString("\\%2_manual_postscan.vol").arg(m_fileNumber));
        portaGetDisplayDimensions(0, hdr.w, hdr.h);
        hdr.type = 1; //BPost;
        hdr.ss = 8;

        szFrm = hdr.w * hdr.h;
        buffer = new unsigned char[szFrm];
    }
    else if (m_rf)
    {
        defaultFile.append(QString("\\%2_manual_rf.vol").arg(m_fileNumber));
        hdr.type = 2; //rf;
        hdr.w = portaGetParam(prmRfNumLines);
        hdr.h = portaGetParam(prmRfNumSamples) >> portaGetParam("b-decimation");
        hdr.ss = 16;

        szFrm = hdr.w * hdr.h * hdr.ss / 8;
        buffer = new unsigned char[szFrm];
    }

    fp = fopen(defaultFile.toAscii().constData(), "wb");
    if (!fp)
    {
        return false;
    }

    // write the data header
    fwrite(&hdr, sizeof(hdr), 1, fp);

    // run manual volume acquisition
    for (i = 0; i < fpv; i++)
    {
        portaRunImage();
        ::Sleep(slptime);
        portaStopImage();

        while (portaGetFrameCount(0) < 1)
        {
            slptime *= 2;
            portaRunImage();
            ::Sleep(slptime);
            portaStopImage();
        }

        if (m_prescan)
        {
            portaGetBwImagePrescan(0, buffer);
            fwrite(buffer, szFrm, 1, fp);
        }
        else if (m_postscan)
        {
            portaProcessCineImage(0, i);
            portaGetBwImage(0, buffer, false);
            fwrite(buffer, szFrm, 1, fp);
        }
        else if (m_rf)
        {
            fwrite(portaGetFrameAddress(0, 0), szFrm, 1, fp);
        }

        portaStepMotor(false, steps);
    }

    if (fp)
    {
        fclose(fp);
    }

    return true;
}

bool SaveDlg::saveSingleFrame()
{
    QString defaultFile;
    unsigned char* buffer;
    uFileHeader uhdr;
    int szFrm;
    FILE* fp = 0;

    uhdr.frames = 1;

    if (m_prescan)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_single_prescan.bpr").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }
        uhdr.type = udtBPre;
        uhdr.ss = 8;

        portaGetPrescanBDimensions(0, uhdr.w, uhdr.h);
        szFrm = uhdr.w * uhdr.h;
        buffer = new unsigned char[szFrm];

        portaGetBwImagePrescan(0, buffer);
        // write ulterius header if requested
        if (m_header)
        {
            fwrite(&uhdr, sizeof(uhdr), 1, fp);
        }
        fwrite(buffer, szFrm, 1, fp);

        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }
    if (m_postscan)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_single_postscan.b8").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }
        uhdr.type = udtBPost;
        uhdr.ss = 8;

        portaGetDisplayDimensions(0, uhdr.w, uhdr.h);
        uhdr.sf = portaGetParam(prmBSamplingFreq);
        szFrm = (uhdr.w * uhdr.h * (uhdr.ss / 8));
        buffer = new unsigned char[szFrm];

        portaProcessCineImage(0, 0);
        portaGetBwImage(0, buffer, (uhdr.ss == 32) ? true : false);
        // write ulterius header if requested
        if (m_header)
        {
            fwrite(&uhdr, sizeof(uhdr), 1, fp);
        }
        fwrite(buffer, szFrm, 1, fp);

        delete[] buffer;
        if (fp)
        {
            fclose(fp);
        }
    }
    if (m_rf)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_single_rf.rf").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }
        uhdr.type = udtRF;
        uhdr.ss = 16;
        uhdr.w = portaGetParam(prmRfNumLines);
        uhdr.h = portaGetParam(prmRfNumSamples) >> portaGetParam("b-decimation");
        uhdr.sf = 40000000 >> portaGetParam("b-decimation");
        szFrm = uhdr.w * uhdr.h * uhdr.ss / 8;

        // write ulterius header if requested
        if (m_header)
        {
            fwrite(&uhdr, sizeof(uhdr), 1, fp);
        }
        fwrite(portaGetFrameAddress(0, 0), szFrm, 1, fp);

        if (fp)
        {
            fclose(fp);
        }
    }
    if (m_colorvv)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_single_colorvv.cvv").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }
        uhdr.type = udtColorVelocityVariance;
        uhdr.ss = 16;

        portaGetDisplayDimensions(0, uhdr.w, uhdr.h);
        szFrm = (uhdr.w * uhdr.h * (uhdr.ss / 8));
        buffer = new unsigned char[szFrm];

        portaGetColorVV(0, buffer);
        // write ulterius header if requested
        if (m_header)
        {
            fwrite(&uhdr, sizeof(uhdr), 1, fp);
        }
        fwrite(buffer, szFrm, 1, fp);

        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }

    if (m_colorpost)
    {
        defaultFile = m_filePath;
        defaultFile.append(QString("\\%2_single_colorpost.col").arg(m_fileNumber));
        fp = fopen(defaultFile.toAscii().constData(), "wb");
        if (!fp)
        {
            return false;
        }
        uhdr.type = udtColorPost;
        uhdr.ss = 32;
        portaGetDisplayDimensions(0, uhdr.w, uhdr.h);
        szFrm = (uhdr.w * uhdr.h * (uhdr.ss / 8));
        buffer = new unsigned char[szFrm];

        portaGetColorImage(0, buffer);
        // write ulterius header if requested
        if (m_header)
        {
            fwrite(&uhdr, sizeof(uhdr), 1, fp);
        }
        fwrite(buffer, szFrm, 1, fp);

        delete[] buffer;

        if (fp)
        {
            fclose(fp);
        }
    }

    return true;
}

void SaveDlg::updateSaveOptions()
{
    //items 0 to 5 are the state
    m_prescan = (wSaveOptions->item(0, 1)->checkState() == Qt::Checked) ? true : false;
    m_postscan = (wSaveOptions->item(0, 2)->checkState() == Qt::Checked) ? true : false;
    m_rf = (wSaveOptions->item(0, 3)->checkState() == Qt::Checked) ? true : false;
    m_colorvv = (wSaveOptions->item(0, 4)->checkState() == Qt::Checked) ? true : false;
    m_colorpost = (wSaveOptions->item(0, 5)->checkState() == Qt::Checked) ? true : false;

    //item 6 is the header
    m_header = (wSaveOptions->item(0, 6)->checkState() == Qt::Checked) ? true : false;
}

void SaveDlg::enableEdit(QTableWidgetItem* pItem)
{
    Qt::ItemFlags eFlags = pItem->flags();
    eFlags |= Qt::ItemIsUserCheckable;
    pItem->setFlags(eFlags);
}

void SaveDlg::disableEdit(QTableWidgetItem* pItem)
{
    Qt::ItemFlags eFlags = pItem->flags();
    eFlags &= ~Qt::ItemIsUserCheckable;
    pItem->setFlags(eFlags);
}
