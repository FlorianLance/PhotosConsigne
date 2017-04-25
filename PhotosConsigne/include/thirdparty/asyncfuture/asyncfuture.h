#pragma once
#include <QFuture>
#include <QMetaMethod>
#include <QPointer>
#include <QThread>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <functional>

namespace AsyncFuture {

/* Naming Convention
 *
 * typename T - The type of observable QFuture
 * typename R - The return type of callback
 */

namespace Private {

// Value is a wrapper of data structure which could contain <void> type.
// AsyncFuture do not use QVariant because it need user to register before use.
template <typename R>
class Value {
public:
    Value() {
    }

    Value(R&& v) : value(v){
    }

    Value(R* v) : value(*v) {
    }

    Value(QFuture<R> future) {
        value = future.result();
    }

    R value;
};

template <>
class Value<void> {
public:
    Value() {
    }

    Value(void*) {
    }

    Value(QFuture<void> future) {
        Q_UNUSED(future);
    }
};

template <typename F>
void runInMainThread(F func) {
    QObject tmp;
    QObject::connect(&tmp, &QObject::destroyed,
                     QCoreApplication::instance(), std::move(func), Qt::QueuedConnection);
}

/*
 * @param owner If the object is destroyed, it should destroy the watcher
 * @param contextObject Determine the receiver callback
 */

template <typename T, typename Finished, typename Canceled>
void watch(QFuture<T> future,
           QObject* owner,
           QObject* contextObject,
           Finished finished,
           Canceled canceled) {


    QFutureWatcher<T> *watcher = new QFutureWatcher<T>();

    if (owner) {
        // Don't set parent as the context object as it may live in different thread
        QObject::connect(owner, &QObject::destroyed,
                         watcher, &QObject::deleteLater);
    }

    if (contextObject) {

        QObject::connect(watcher, &QFutureWatcher<T>::finished,
                         contextObject, [=]() {
            watcher->disconnect();
            watcher->deleteLater();
            finished();
        });

        QObject::connect(watcher, &QFutureWatcher<T>::canceled,
                         contextObject, [=]() {
            watcher->disconnect();
            watcher->deleteLater();
            canceled();
        });

    } else {

        QObject::connect(watcher, &QFutureWatcher<T>::finished,
                         [=]() {
            watcher->disconnect();
            watcher->deleteLater();
            finished();
        });

        QObject::connect(watcher, &QFutureWatcher<T>::canceled,
                         [=]() {
            watcher->disconnect();
            watcher->deleteLater();
            canceled();
        });
    }

    if ((QThread::currentThread() != QCoreApplication::instance()->thread()) &&
         (contextObject == 0 || QThread::currentThread() != contextObject->thread())) {
        // Move watcher to main thread
        watcher->moveToThread(QCoreApplication::instance()->thread());
    }

    watcher->setFuture(future);
}

template <typename T>
class DeferredFuture : public QObject, public QFutureInterface<T>{
public:
    DeferredFuture(QObject* parent = 0): QObject(parent),
                                         QFutureInterface<T>(QFutureInterface<T>::Running),
                                         autoDelete(false),
                                         resolved(false),
                                         refCount(1) {
    }

    ~DeferredFuture() {
        cancel();
    }

    void complete() {
        if (resolved) {
            return;
        }
        resolved = true;
        QFutureInterface<T>::reportFinished();

        if (autoDelete) {
            deleteLater();
        }
    }

    template <typename R>
    void complete(R value) {
        if (resolved) {
            return;
        }
        resolved = true;
        reportResult(value);
        QFutureInterface<T>::reportFinished();

        if (autoDelete) {
            deleteLater();
        }
    }

    template <typename R>
    void complete(Value<R> value) {
        this->complete(value.value);
    }

    void complete(Value<void> value) {
        Q_UNUSED(value);
        this->complete();
    }

    void complete(QFuture<T> future) {
        incRefCount();
        auto onFinished = [=]() {
            Value<T> value(future);
            this->complete(value);
            this->decRefCount();
        };

        auto onCanceled = [=]() {
            this->cancel();
            this->decRefCount();
        };

        watch(future,
              this,
              0,
              onFinished,
              onCanceled);
    }


    void cancel() {
        if (resolved) {
            return;
        }
        resolved = true;
        QFutureInterface<T>::reportCanceled();
        QFutureInterface<T>::reportFinished();

        if (autoDelete) {
            this->deleteLater();
        }
    }

    template <typename Member>
    void cancel(QObject* sender, Member member) {

        QObject::connect(sender, member,
                         this, [=]() {
            this->cancel();
        });
    }

    template <typename ANY>
    void cancel(QFuture<ANY> future) {
        incRefCount();
        auto onFinished = [=]() {
            cancel();
            decRefCount();
        };

        auto onCanceled = [=]() {
            decRefCount();
        };

        watch(future,
              this,
              0,
              onFinished,
              onCanceled);
    }

    void incRefCount() {
        refCount++;
    }

    void decRefCount() {
        refCount--;
        if (refCount <= 0) {
            cancel();

            // In case it is already resolved
            if (autoDelete) {
                deleteLater();
            }
        }
    }

    /// Create a DeferredFugture instance in a shared pointer
    static QSharedPointer<DeferredFuture<T> > create() {

        auto deleter = [](DeferredFuture<T> *object) {
            if (object->resolved) {
                // If that is already resolved, it is not necessary to keep it in memory
                object->deleteLater();
            } else {
                object->autoDelete = true;
                object->decRefCount();
            }
        };

        QSharedPointer<DeferredFuture<T> > ptr(new DeferredFuture<T>(), deleter);
        return ptr;
    }

    // Enable auto delete if the refCount is dropped to zero or it is completed/canceled
    bool autoDelete;
    bool resolved;

    // A virtual reference count system. If autoDelete is not true, it won't delete the object even the count is zero
    int refCount;

protected:

    void reportResult(Value<void> &value) {
        Q_UNUSED(value);
    }

    template <typename R>
    void reportResult(R& value) {
        QFutureInterface<T>::reportResult(value);
    }

    template <typename R>
    void reportResult(Value<R>& value) {
        QFutureInterface<T>::reportResult(value.value);
    }

};

class CombinedFuture: public DeferredFuture<void> {
public:
    CombinedFuture(bool settleAllMode = false) : DeferredFuture<void>(), settleAllMode(settleAllMode) {
        settledCount = 0;
        count = 0;
        anyCanceled = false;
    }

    template <typename T>
    void addFuture(const QFuture<T> future) {
        if (resolved) {
            return;
        }
        int index = count++;
        incRefCount();

        Private::watch(future, this, 0,
                       [=]() {
            completeFutureAt(index);
            decRefCount();
        },[=]() {
            cancelFutureAt(index);
            decRefCount();
        });
    }

    int settledCount;
    int count;
    bool anyCanceled;
    bool settleAllMode;

    static QSharedPointer<CombinedFuture> create(bool settleAllMode) {

        auto deleter = [](CombinedFuture *object) {
            if (object->resolved) {
                // If that is already resolved, it is not necessary to keep it in memory
                object->deleteLater();
            } else {
                object->autoDelete = true;
                object->decRefCount();
            }
        };

        QSharedPointer<CombinedFuture> ptr(new CombinedFuture(settleAllMode), deleter);
        return ptr;
    }

private:

    void completeFutureAt(int index) {
        Q_UNUSED(index);
        settledCount++;
        checkFulfilled();
    }

    void cancelFutureAt(int index) {
        Q_UNUSED(index);
        settledCount++;
        anyCanceled = true;
        checkFulfilled();
    }

    void checkFulfilled() {
        if (resolved) {
            return;
        }

        if (anyCanceled && !settleAllMode) {
            cancel();
            return;
        }

        if (settledCount == count) {
            if (anyCanceled) {
                cancel();
            } else {
                complete();
            }
        }
    }

};

template <typename ARG>
class Proxy : public QObject {
public:
    Proxy(QObject* parent) : QObject(parent) {
    }

    QVector<int> parameterTypes;
    std::function<void(Value<ARG>)> callback;
    QMetaObject::Connection conn;
    QPointer<QObject> sender;

    template <typename Method>
    void bind(QObject* source, Method pointToMemberFunction) {
        sender = source;

        const int memberOffset = QObject::staticMetaObject.methodCount();

        QMetaMethod method = QMetaMethod::fromSignal(pointToMemberFunction);

        parameterTypes = QVector<int>(method.parameterCount());

        for (int i = 0 ; i < method.parameterCount() ; i++) {
            parameterTypes[i] = method.parameterType(i);
        }

        conn = QMetaObject::connect(source, method.methodIndex(), this, memberOffset, Qt::QueuedConnection, 0);

        if (!conn) {
            qWarning() << "AsyncFuture::Private::Proxy: Failed to bind signal";
        }
    }

    int qt_metacall(QMetaObject::Call _c, int _id, void **_a) {
        int methodId = QObject::qt_metacall(_c, _id, _a);

        if (methodId < 0) {
            return methodId;
        }

        if (_c == QMetaObject::InvokeMetaMethod) {
            if (methodId == 0) {
                sender->disconnect(conn);
                if (parameterTypes.count() > 0) {

                    Value<ARG> value(reinterpret_cast<ARG*>(_a[1]));
                    callback(value);
                } else {
                    // It is triggered only if ARG==void.
                    callback(Value<ARG>((ARG*) 0));
                }
            }
        }
        return methodId;
    }
};

// Determine is the input type a QFuture
template <typename T>
struct future_traits {
    enum {
        is_future = 0
    };

    typedef void arg_type;
};

template <template <typename> class C, typename T>
struct future_traits<C <T> >
{
    enum {
        is_future = 0
    };

    typedef void arg_type;
};

template <typename T>
struct future_traits<QFuture<T> >{
    enum {
        is_future = 1
    };
    typedef T arg_type;
};

// function_traits: Source: http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

template <typename T>
struct function_traits
        : public function_traits<decltype(&T::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    enum { arity = sizeof...(Args) };
    // arity is the number of arguments.

    typedef ReturnType result_type;

    enum {
        result_type_is_future = future_traits<result_type>::is_future
    };

    // If the result_type is a QFuture<T>, the type will be T. Otherwise, it is void
    typedef typename future_traits<result_type>::arg_type future_arg_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
        // the i-th argument is equivalent to the i-th tuple element of a tuple
        // composed of those arguments.
    };
};

template <typename T>
struct signal_traits {
};

template <typename R, typename C>
struct signal_traits<R (C::*)()> {
    typedef void result_type;
};

template <typename R, typename C, typename ARG1>
struct signal_traits<R (C::*)(ARG1)> {
    typedef ARG1 result_type;
};

template <typename Functor, typename T>
typename std::enable_if<!(std::is_same<T, void>::value || Private::function_traits<Functor>::arity != 1) ,
Value<typename Private::function_traits<Functor>::result_type>>::type
    invoke(Functor functor, QFuture<T> future) {
    Value<typename Private::function_traits<Functor>::result_type> value(functor(future));
    return value;
}

template <typename Functor, typename T>
typename std::enable_if<(std::is_same<T, void>::value || Private::function_traits<Functor>::arity != 1) ,
Value<typename Private::function_traits<Functor>::result_type>>::type
    invoke(Functor functor, QFuture<T> future) {
    Q_UNUSED(future);
    static_assert(Private::function_traits<Functor>::arity == 0, "Your callback should not take any argument because the observed type is QFuture<void>");
    Value<typename Private::function_traits<Functor>::result_type> value(functor());
    return value;
}

template <typename Functor, typename T>
typename std::enable_if<!(std::is_same<T, void>::value || Private::function_traits<Functor>::arity != 1) ,
void>::type
voidInvoke(Functor functor, QFuture<T> future) {
    functor(future);
}

template <typename Functor, typename T>
typename std::enable_if<(std::is_same<T, void>::value || Private::function_traits<Functor>::arity != 1) ,
void>::type
voidInvoke(Functor functor, QFuture<T> future) {
    Q_UNUSED(future);
    functor();
    /* Toubleshooting Tips:
     * 1) Are you observing a QFuture<void> but require an input argument in your callback function?
     */
}

template <typename Functor, typename T>
typename std::enable_if<!std::is_same<typename Private::function_traits<Functor>::result_type, void>::value,
Value<typename Private::function_traits<Functor>::result_type>>::type
run(Functor functor, QFuture<T> future) {
    auto value = invoke(functor,future);
    return value;
}

template <typename Functor, typename T>
typename std::enable_if<std::is_same<typename Private::function_traits<Functor>::result_type, void>::value,
Value<void>>::type
run(Functor functor, QFuture<T> future) {
    voidInvoke(functor, future);
    return Value<void>();
}

/// Create a DeferredFuture will execute the callback functions when observed future finished
/** DeferredType - The template type of the DeferredType
 *  RetType - The return type of QFuture
 *
 * DeferredType and RetType can be different.
 * e.g DeferredFuture<int> = Value<QFuture<int>>
 */
template <typename DeferredType, typename RetType, typename T, typename Completed, typename Canceled>
static DeferredFuture<DeferredType>* execute(QFuture<T> future, QObject* contextObject, Completed onCompleted, Canceled onCanceled) {

    DeferredFuture<DeferredType>* defer = new DeferredFuture<DeferredType>();
    defer->autoDelete = true;
    watch(future,
          contextObject,
          contextObject,[=]() {
        Value<RetType> value = run(onCompleted, future);
        defer->complete(value);
    }, [=]() {
        onCanceled();
        defer->cancel();
    });

    if (contextObject) {
        defer->cancel(contextObject, &QObject::destroyed);
    }

    return defer;
}


} // End of Private Namespace

template <typename T>
class Observable {
protected:
    QFuture<T> m_future;

public:

    Observable() {

    }

    Observable(QFuture<T> future) {
        m_future = future;
    }

    QFuture<T> future() const {
        return m_future;
    }

    template <typename Functor>
    typename std::enable_if< !Private::future_traits<typename Private::function_traits<Functor>::result_type>::is_future,
    Observable<typename Private::function_traits<Functor>::result_type>
    >::type
    context(QObject* contextObject, Functor functor)  {
        /* functor return non-QFuture type */
        return _context<typename Private::function_traits<Functor>::result_type,
                       typename Private::function_traits<Functor>::result_type
                >(contextObject, functor);
    }

    template <typename Functor>
    typename std::enable_if< Private::future_traits<typename Private::function_traits<Functor>::result_type>::is_future,
    Observable<typename Private::future_traits<typename Private::function_traits<Functor>::result_type>::arg_type>
    >::type
    context(QObject* contextObject, Functor functor)  {
        /* functor returns a QFuture */
        return _context<typename Private::future_traits<typename Private::function_traits<Functor>::result_type>::arg_type,
                       typename Private::function_traits<Functor>::result_type
                >(contextObject, functor);
    }

    template <typename Completed, typename Canceled>
    typename std::enable_if< !Private::future_traits<typename Private::function_traits<Completed>::result_type>::is_future,
    Observable<typename Private::function_traits<Completed>::result_type>
    >::type
    subscribe(Completed onCompleted,
              Canceled onCanceled) {
        /* For functor return a regular value */
        return _subscribe<typename Private::function_traits<Completed>::result_type,
                         typename Private::function_traits<Completed>::result_type
                >(onCompleted, onCanceled);
    }

    template <typename Completed>
    typename std::enable_if< !Private::future_traits<typename Private::function_traits<Completed>::result_type>::is_future,
    Observable<typename Private::function_traits<Completed>::result_type>
    >::type
    subscribe(Completed onCompleted) {
        return _subscribe<typename Private::function_traits<Completed>::result_type,
                         typename Private::function_traits<Completed>::result_type
                >(onCompleted, [](){});
    }


    template <typename Completed, typename Canceled>
    typename std::enable_if<Private::future_traits<typename Private::function_traits<Completed>::result_type>::is_future,
    Observable<typename Private::function_traits<Completed>::future_arg_type>
    >::type
    subscribe(Completed onCompleted,
              Canceled onCanceled) {
        /* onCompleted returns a QFuture */
        return _subscribe<typename Private::future_traits<typename Private::function_traits<Completed>::result_type>::arg_type,
                         typename Private::function_traits<Completed>::result_type
                >(onCompleted, onCanceled);
    }

    template <typename Completed>
    typename std::enable_if<Private::future_traits<typename Private::function_traits<Completed>::result_type>::is_future,
    Observable<typename Private::function_traits<Completed>::future_arg_type>
    >::type
    subscribe(Completed onCompleted) {
        /* onCompleted returns a QFuture */

        return _subscribe<typename Private::future_traits<typename Private::function_traits<Completed>::result_type>::arg_type,
                         typename Private::function_traits<Completed>::result_type
                >(onCompleted, [](){});
    }


private:
    template <typename ObservableType, typename RetType, typename Functor>
    Observable<ObservableType> _context(QObject* contextObject, Functor functor)  {

        static_assert(Private::function_traits<Functor>::arity <= 1, "context(): Callback should take not more than one parameter");

        auto defer = Private::execute<ObservableType, RetType>(m_future,
                                                               contextObject,
                                                               functor,
                                                               [](){});

        return Observable<ObservableType>(defer->future());
    }

    template <typename ObservableType, typename RetType, typename Completed, typename Canceled>
    Observable<ObservableType> _subscribe(Completed onCompleted, Canceled onCanceled) {

        auto defer = Private::execute<ObservableType, RetType>(m_future,
                                                               0,
                                                               onCompleted,
                                                               onCanceled);

        return Observable<ObservableType>(defer->future());
    }

};

template <typename T>
class Deferred : public Observable<T> {

public:
    Deferred() : Observable<T>(),
              defer(Private::DeferredFuture<T>::create())  {
        this->m_future = defer->future();
    }

    void complete(QFuture<T> future) {
        defer->complete(future);
    }

    void complete(T value)
    {
        defer->complete(value);
    }

    template <typename ANY>
    void cancel(QFuture<ANY> future) {
        defer->cancel(future);
    }


    void cancel() {
        defer->cancel();
    }

private:
    QSharedPointer<Private::DeferredFuture<T> > defer;
};

template<>
class Deferred<void> : public Observable<void> {

public:
    Deferred() : Observable<void>(),
              defer(Private::DeferredFuture<void>::create())  {
        this->m_future = defer->future();
    }

    void complete(QFuture<void> future) {
        defer->complete(future);
    }

    void complete() {
        defer->complete();
    }

    template <typename ANY>
    void cancel(QFuture<ANY> future) {
        defer->cancel(future);
    }

    void cancel() {
        defer->cancel();
    }

private:
    QSharedPointer<Private::DeferredFuture<void> > defer;
};

typedef enum {
    FailFast,
    AllSettled
} CombinatorMode;

class Combinator : public Observable<void> {
private:
    QSharedPointer<Private::CombinedFuture> combinedFuture;

public:
    inline Combinator(CombinatorMode mode = FailFast) : Observable<void>() {
        combinedFuture = Private::CombinedFuture::create(mode == AllSettled);
        m_future = combinedFuture->future();
    }
    inline ~Combinator() {
        if (!combinedFuture.isNull() && combinedFuture->count == 0) {
            // No future added
            combinedFuture->deleteLater();
        }
    }

    template <typename T>
    Combinator& combine(QFuture<T> future) {
        combinedFuture->addFuture(future);
        return *this;
    }

    template <typename T>
    Combinator& operator<<(QFuture<T> future) {
        combinedFuture->addFuture(future);
        return *this;
    }
};

template <typename T>
static Observable<T> observe(QFuture<T> future) {
    return Observable<T>(future);
}

template <typename Member>
auto observe(QObject* object, Member pointToMemberFunction)
-> Observable< typename Private::signal_traits<Member>::result_type> {

    typedef typename Private::signal_traits<Member>::result_type RetType;

    auto defer = new Private::DeferredFuture<RetType>();

    auto proxy = new Private::Proxy<RetType>(defer);

    defer->autoDelete = true;

    defer->cancel(object, &QObject::destroyed);

    proxy->bind(object, pointToMemberFunction);
    proxy->callback = [=](Private::Value<RetType> value) {
        defer->complete(value);
        proxy->deleteLater();
    };

    Observable< typename Private::signal_traits<Member>::result_type> observer(defer->future());
    return observer;
}

template <typename T>
auto deferred() -> Deferred<T> {
    return Deferred<T>();
}

inline Combinator combine(CombinatorMode mode = FailFast) {
    return Combinator(mode);
}

}
