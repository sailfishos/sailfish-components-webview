// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <QObject>
namespace Silica {
class Theme : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal pixelRatio READ pixelRatio CONSTANT)
    Q_PROPERTY(ColorScheme colorScheme READ colorScheme NOTIFY colorSchemeChanged)
public:
    enum ColorScheme { LightOnDark, DarkOnLight };
    Q_ENUM(ColorScheme)
    explicit Theme(QObject *parent = nullptr) : QObject(parent) {}
    qreal pixelRatio() const { return 1.0; }
    ColorScheme colorScheme() const { return m_scheme; }
    void setColorScheme(ColorScheme scheme) {
        if (m_scheme != scheme) {
            m_scheme = scheme;
            emit colorSchemeChanged();
        }
    }
private:
    ColorScheme m_scheme = LightOnDark;
signals:
    void colorSchemeChanged();
};
}
