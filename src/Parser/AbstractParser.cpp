#include <Parser/AbstractParser.h>

namespace Parser {
    AbstractParser::AbstractParser() {

    }

    QUrl AbstractParser::file() const {
        return QUrl();
    }

    QVariant AbstractParser::files() const {
        return QVariant();
    }

    QVariant AbstractParser::blueprint() const {
        return _blueprint;
    }

    void AbstractParser::setFile(const QUrl & file) {
        Q_UNUSED(file)

        emit fileChanged();
    }

    void AbstractParser::setFiles(const QVariant & files) {
        Q_UNUSED(files)

        emit filesChanged();
    }

    void AbstractParser::setBlueprint(const QVariant & blueprint) {
        _blueprint = blueprint;

        emit blueprintChanged(blueprint);
    }
}
