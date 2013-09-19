/*
Copyright 2009 Will Stephenson <wstephenson@kde.org>
Copyright 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLASMANM_STRONGSWAN_H
#define PLASMANM_STRONGSWAN_H

#include "vpnuiplugin.h"

#include <QVariant>

class StrongswanUiPlugin : public VpnUiPlugin
{
Q_OBJECT
public:
    explicit StrongswanUiPlugin(QObject * parent = 0, const QVariantList& = QVariantList());
    virtual ~StrongswanUiPlugin();
    virtual SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    virtual SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    virtual QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const;
    virtual QString supportedFileExtensions() const;

    virtual NMVariantMapMap importConnectionSettings(const QString &fileName);
    virtual bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName);
};

#endif //  PLASMANM_STRONGSWAN_H
