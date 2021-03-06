#include "textureio.h"

#include "textureiohandler.h"
#include "textureiohandlerdatabase.h"
#include "textureioresult.h"

#include <OptionalType>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QMimeDatabase>

using Capability = TextureIOHandlerPlugin::Capability;
using Capabilities = TextureIOHandlerPlugin::Capabilities;

static inline QIODevice::OpenMode caps2OpenMode(Capabilities caps)
{
    QIODevice::OpenMode result;
    if (caps & Capability::CanRead)
        result |= QIODevice::ReadOnly;
    if (caps & Capability::CanWrite)
        result |= QIODevice::WriteOnly;
    return result;
}

class TextureIOPrivate
{
    Q_DECLARE_PUBLIC(TextureIO)
    TextureIO *q_ptr {nullptr};

public:
    using QIODevicePointer = TextureIO::QIODevicePointer;

    explicit TextureIOPrivate(TextureIO *qq) : q_ptr(qq) {}

    TextureIOResult ensureDeviceOpened(Capabilities caps);
    TextureIOResult ensureHandlerCreated(Capabilities caps);
    void resetHandler();

    std::unique_ptr<TextureIOHandler> handler;

    QString fileName;
    std::unique_ptr<QFile> file;

    QIODevicePointer device;
    Optional<QMimeType> mimeType {};
};

TextureIOResult TextureIOPrivate::ensureDeviceOpened(Capabilities caps)
{
    if (!device)
        return TextureIOError::DeviceError;

    const auto mode = caps2OpenMode(caps);

    if (!(device->openMode() & mode)) {
        if (!device->open(mode | device->openMode())) {
            if ((mode & QIODevice::ReadOnly) && file && !file->exists())
                return TextureIOError::FileNotFound;
            return TextureIOError::DeviceError;
        }
    }

    return TextureIOResult();
}

TextureIOResult TextureIOPrivate::ensureHandlerCreated(Capabilities caps)
{
    const auto ok = ensureDeviceOpened(caps);
    if (!ok)
        return ok;

    if (handler)
        return TextureIOResult();

    auto mt = QMimeType();
    if (!mimeType) {
        // mimeType is not set, try to guess from file
        if (file)
            mt = QMimeDatabase().mimeTypeForFile(fileName);
        else if ((device->openMode() & QIODevice::ReadOnly))
            mt = QMimeDatabase().mimeTypeForData(device->peek(256));
    } else {
        mt = *mimeType;
    }

    if (!mt.isValid())
        return TextureIOError::InvalidMimeType;

    auto db = TextureIOHandlerDatabase::instance();
    handler = db->create(device, mt.name(), caps);
    if (!handler)
        return TextureIOError::UnsupportedMimeType;

    return TextureIOResult();
}

void TextureIOPrivate::resetHandler()
{
    handler.reset();
}

/*!
    \class TextureIO
    \brief TextureIO implements Texture loading and saving.
*/

/*!
  \brief Constructs a TextureIO object.
*/
TextureIO::TextureIO() :
    d_ptr(new TextureIOPrivate(this))
{
}

/*!
  \brief Move-constructs a TextureIO object from the \a other object.
*/
TextureIO::TextureIO(TextureIO &&other) noexcept
{
    d_ptr.swap(other.d_ptr);
}

/*!
  \brief Constructs a TextureIO object with the given \a fileName and \a mimeType.
*/
TextureIO::TextureIO(const QString &fileName, const QMimeType &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setFileName(fileName);
    if (mimeType.isValid()) // invalid mimetype will overwrite detected type
        setMimeType(mimeType);
}

/*!
  \brief Constructs a TextureIO object with the given \a device and \a mimeType.
*/
TextureIO::TextureIO(QIODevicePointer device, const QMimeType &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setDevice(device);
    setMimeType(mimeType);
}

/*!
  \brief Constructs a TextureIO object with the given \a fileName and \a mimeType.
*/
TextureIO::TextureIO(const QString &fileName, QStringView mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setFileName(fileName);
    if (!mimeType.isEmpty()) // invalid mimetype will overwrite detected type
        setMimeType(mimeType);
}

/*!
  \brief Constructs a TextureIO object with the given \a device and \a mimeType.
*/
TextureIO::TextureIO(QIODevicePointer device, QStringView mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setDevice(device);
    setMimeType(mimeType);
}

/*!
  \brief Destroys the TextureIO object.
*/
TextureIO::~TextureIO() = default;

/*!
  \brief Move-assigns \a other to this TextureIO instance.
*/
TextureIO &TextureIO::operator=(TextureIO &&other) noexcept
{
    if (this == &other)
        return *this;
    d_ptr.swap(other.d_ptr);
    return *this;
}

/*!
  \property TextureIO::fileName
  \brief This property holds the filename that is set to this TextureIO object.
*/

QString TextureIO::fileName() const
{
    Q_D(const TextureIO);

    return d->fileName;
}

void TextureIO::setFileName(const QString &fileName)
{
    Q_D(TextureIO);

    if (d->fileName == fileName)
        return;

    d->file = std::make_unique<QFile>(fileName);
    d->device.reset(d->file.get());
    d->fileName = fileName;
    d->resetHandler();
}

/*!
  \property TextureIO::device
  \brief This property holds the device that is set to this TextureIO object.
*/

TextureIO::QIODevicePointer TextureIO::device() const
{
    Q_D(const TextureIO);

    return d->device;
}

void TextureIO::setDevice(QIODevicePointer device)
{
    Q_D(TextureIO);

    d->file.reset();
    d->device = device;
    d->resetHandler();
}

/*!
  \property TextureIO::mimeType
  \brief This property holds the mime type that is set to this TextureIO object.

  If no mime type is set, it is automatically determined from the device contents before reading.
*/

QMimeType TextureIO::mimeType() const
{
    Q_D(const TextureIO);
    return d->mimeType ? *d->mimeType : QMimeType();
}

void TextureIO::setMimeType(const QMimeType &mimeType)
{
    Q_D(TextureIO);
    if (mimeType.isValid())
        d->mimeType = mimeType;
    else
        d->mimeType = nullOptional();
    d->resetHandler();
}

void TextureIO::setMimeType(QStringView mimeType)
{
    Q_D(TextureIO);
    if (!mimeType.isEmpty())
        // here we can have optional pointing to an invalid QMimeType,
        // leads to error in ensureHandlerCreated
        d->mimeType = QMimeDatabase().mimeTypeForName(mimeType.toString()); // oops extra allocation
    else
        d->mimeType = nullOptional();
    d->resetHandler();
}

/*!
  \brief Reads the contents of an texture file.

  Returns the status of the operation.
*/
TextureIO::ReadResult TextureIO::read()
{
    Q_D(TextureIO);

    auto ok = d->ensureHandlerCreated(Capability::CanRead);
    if (!ok)
        return makeUnexpected(ok.error());

    Texture texture;
    if (!d->handler->read(texture))
        ok = TextureIOError::HandlerError;

    if (!ok)
        return makeUnexpected(ok.error());

    return texture;
}

/*!
  \brief Writes the given \a contents with the given \a options to the device.

  Returns the status of the operation.
*/
TextureIO::WriteResult TextureIO::write(const Texture &contents)
{
    Q_D(TextureIO);
    auto ok = d->ensureHandlerCreated(Capability::CanWrite);
    if (!ok)
        return ok;

    if (!d->handler->write(contents))
        return TextureIOError::HandlerError;

    if (d->file)
        d->file->flush();
    return WriteResult();
}

gsl::span<const TextureFormat> TextureIO::readableFormats()
{
    return TextureIOHandlerDatabase::instance()->readableFormats();
}

gsl::span<const TextureFormat> TextureIO::writableFormats()
{
    return TextureIOHandlerDatabase::instance()->writableFormats();
}

QString TextureIO::pluginsDirPath()
{
#if defined(Q_OS_LINUX)
    return QStringLiteral("/../lib/textureviewer/plugins/");
#elif defined(Q_OS_MAC)
    return QStringLiteral("/../PlugIns/");
#elif defined(Q_OS_WIN)
    return QStringLiteral("/plugins/");
#else
    return QString();
#endif
}
