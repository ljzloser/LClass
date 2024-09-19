#include "LInstanceMap.h"
#include <QThread>
using namespace ljz;

ljz::Content::Content(QString key, const QVariant &value, const QVariant &oldValue, Type type, const QDateTime& time)
    : _key(std::move(key)), _value(value), _oldValue(oldValue), _type(type), _time(QDateTime::currentDateTime())
{

}

ljz::Content::~Content()
= default;

QString ljz::Content::toString() const
{
    return QString(
        R"(
            {
                "key":"%1",
                "value":"%2",
                "oldValue":"%3",
                "type":"%4",
                "time":"%5"
            }
        )"
    ).arg(_key).arg(_value.toString()).arg(_oldValue.toString()).arg(_type).arg(_time.toString("yyyy-MM-dd hh:mm:ss"));
}

ljz::LInstanceMap& ljz::LInstanceMap::instance()
{
    static LInstanceMap _instance; // 单例
    return _instance;
}
void ljz::LInstanceMap::set(const QString &key, const QVariant &value, bool isRegister)
{
    _lock.lockForWrite();
    bool keyExist = this->contains(key);
    if(lastOp != "remove" && !keyExist)
    {
       auto a = 1;
    }
    // qDebug() << (keyExist ? "Update" : "Insert");
    lastOp = (keyExist ? "Update" : "Insert");

    Content content(key, value, _map.value(key), keyExist ? Content::Type::Update : Content::Type::Insert);
    content._threadId = static_cast<int>(reinterpret_cast<uintptr_t>(QThread::currentThreadId()));
    _map.insert(key, value);

    if (isRegistered(key) || isRegister)
    {
        emit valueChanged(content);
    }
    _lock.unlock();
    if (isRegister)
    {
        this->registerKey(QStringList() << key);
    }
}
void ljz::LInstanceMap::set(const QMap<QString, QVariant> &map, bool isRegister)
{
    for (auto it = map.begin(); it != map.end(); it++)
    {
        this->set(it.key(), it.value(), isRegister);
    }
}
QVariant ljz::LInstanceMap::get(const QString &key)
{
    _lock.lockForRead();
    QVariant value = _map.value(key);
    _lock.unlock();
    return value;
}
QVariantList ljz::LInstanceMap::get(const QStringList &keys)
{
    QVariantList list;
    for (const auto & key : keys)
    {
        list.append(this->get(key));
    }
    return list;
}
ljz::LInstanceMap::LInstanceMap()
    :QObject(nullptr)
{
}

ljz::LInstanceMap::~LInstanceMap()
= default;

void ljz::LInstanceMap::remove(const QString &key, bool isUnRegister)
{
    _lock.lockForWrite();
    if (!this->contains(key))
    {
        _lock.unlock();
        return;
    }
    // qDebug() << "remove";
    lastOp = "remove";
    _map.remove(key);
    if (isRegistered(key))
    {
        Content content(key, QVariant(), _map.value(key), Content::Type::Delete);
        content._threadId = static_cast<int>(reinterpret_cast<uintptr_t>(QThread::currentThreadId()));
        emit valueChanged(content);
    }
    _lock.unlock();
 
    if (isUnRegister)
    {
        this->unRegisterKey(QStringList() << key);
    }
}

void ljz::LInstanceMap::remove(const QStringList &keys, bool isUnRegister)
{
    for (const auto & key : keys)
    {
        this->remove(key, isUnRegister);
    }
}

void ljz::LInstanceMap::clear()
{
    for(auto it = _map.begin(); it != _map.end(); it++)
    {
        this->remove(it.key());
    }
}

int ljz::LInstanceMap::count()
{
    QMutexLocker locker(&_mutex);
    return _map.count();
}

QStringList ljz::LInstanceMap::keys()
{
    QMutexLocker locker(&_mutex);
    return _map.keys();
}

bool ljz::LInstanceMap::contains(const QString &key)
{
    QMutexLocker locker(&_mutex);
    return _map.contains(key);
}

bool ljz::LInstanceMap::isEmpty()
{
    QMutexLocker locker(&_mutex);
    return _map.isEmpty();
}

QMap<QString, QVariant> ljz::LInstanceMap::map()
{
    return _map;
}

void ljz::LInstanceMap::registerkey(const QString key)
{
    QMutexLocker locker(&_mutex);
    _keySet.insert(key);
}

void ljz::LInstanceMap::registerKey(const QStringList &keys)
{
    this->registerKey(QSet<QString>(keys.begin(), keys.end()));
}

void ljz::LInstanceMap::registerKey(const QSet<QString> &keys)
{
    QMutexLocker locker(&_mutex);
    _keySet.unite(keys);
}

void ljz::LInstanceMap::unRegisterKey(const QString &key)
{
    QMutexLocker locker(&_mutex);
    _keySet.remove(key);
}

void ljz::LInstanceMap::unRegisterKey(const QStringList &keys)
{
    this->unRegisterKey(QSet<QString>(keys.begin(), keys.end()));
}

void ljz::LInstanceMap::unRegisterKey(const QSet<QString> &keys)
{
    QMutexLocker locker(&_mutex);
    _keySet.subtract(keys);
}

QStringList ljz::LInstanceMap::registeredKeys()
{
    QMutexLocker locker(&_mutex);
    return {_keySet.begin(), _keySet.end()};
}

bool ljz::LInstanceMap::isRegistered(const QString &key)
{
    QMutexLocker locker(&_mutex);
    return _keySet.contains(key);
}
