// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <QObject>
namespace Silica {
class Theme : public QObject {
    Q_OBJECT
public:
    enum ColorScheme { LightOnDark, DarkOnLight };
    static Theme *instance() { static Theme theme; return &theme; }
    qreal pixelRatio() const { return 1.0; }
    ColorScheme colorScheme() const { return DarkOnLight; }
signals:
    void colorSchemeChanged();
};
}
