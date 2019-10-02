#ifndef FILE_CONVERT_H
#define FILE_CONVERT_H

#include <QObject>

class FileConvert : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(int process READ getProcess WRITE setProcess NOTIFY processChanged)
public:
    explicit FileConvert(QObject *parent = nullptr);

    QString getFileName() const{return fileName;}
    void setFileName(QString s){fileName = s; emit fileNameChanged();}

    int getProcess() const{return process;}
    void setProcess(int v){process = v; emit processChanged();}

signals:
    void fileNameChanged();
    void processChanged();

public slots:
public:
    Q_INVOKABLE void doConvert(int type);
private:
    QString fileName;
    int process;
    int txt2bin(FILE* fpin,FILE* fpout, long size);
    int bin2txt(FILE* fpin,FILE* fpout, long size);
};

#endif // FILE_CONVERT_H
