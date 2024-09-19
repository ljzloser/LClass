#include <QMap>
#include <QReadWriteLock>
#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QSet>
#include "lcore_global.h"
#include <QThread>
namespace ljz
{
    // 更新内容
    class LCORE_EXPORT Content
    {
    public:
    enum Type
    {
        Insert = 0,
        Update = 1,
        Delete = 2
    };
        Type _type;
        QString _key;
        QVariant _value;
        QVariant _oldValue;
        QDateTime _time;
        int _threadId;
        Content(QString  key, const QVariant& value, const QVariant& oldValue,Type type = Type::Insert,const QDateTime& time = QDateTime::currentDateTime());
        ~Content();
        QString toString() const;
    };
    //单例类
    class LCORE_EXPORT LInstanceMap : public QObject
    {
        Q_OBJECT
    public:
        static LInstanceMap& instance();
        void set(const QString& key, const QVariant& value,bool isRegister = false);
        void set (const QMap<QString, QVariant>& map ,bool isRegister = false);
        QVariant get(const QString& key);
        QVariantList get(const QStringList& keys);
        void remove(const QString& key,bool isUnRegister = true);
        void remove(const QStringList& keys,bool isUnRegister = true);
        void clear();
        int count();
        QStringList keys();
        bool contains(const QString& key);
        bool isEmpty();
        QMap<QString, QVariant> map();
        // 注册需要监听的key
        void registerkey(const QString key);
        void registerKey(const QStringList& keys);
        void registerKey(const QSet<QString>& keys);
        // 取消注册
        void unRegisterKey(const QString& key);
        void unRegisterKey(const QStringList& keys);
        void unRegisterKey(const QSet<QString>& keys);
        // 获取当前需要监听的key
        QStringList registeredKeys();
        bool isRegistered(const QString& key);
    signals:
        void valueChanged(Content content);
    private:
        void valueChange(Content content);
        QReadWriteLock _lock;
        QMutex _mutex;
        QMap<QString, QVariant> _map; // 实际数据
        QSet<QString> _keySet; // 需要监听的key
        LInstanceMap();
        ~LInstanceMap();
        QDateTime time;
    };
}
