/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "iodinewidget.h"
#include "ui_iodine.h"

#include "nm-iodine-service.h"

#include <NetworkManagerQt/Setting>

#include <QDBusMetaType>

IodineWidget::IodineWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent, Qt::WindowFlags f)
    : SettingWidget(setting, parent, f)
    , m_ui(new Ui::IodineWidget)
    , m_setting(setting)
{
    qDBusRegisterMetaType<NMStringMap>();

    m_ui->setupUi(this);

    m_ui->le_password->setPasswordOptionsEnabled(true);

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(m_ui->le_toplevelDomain, &QLineEdit::textChanged, this, &IodineWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

IodineWidget::~IodineWidget()
{
    delete m_ui;
}

void IodineWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    const NMStringMap data = m_setting->data();

    const QString toplevelDomain = data.value(NM_IODINE_KEY_TOPDOMAIN);
    if (!toplevelDomain.isEmpty()) {
        m_ui->le_toplevelDomain->setText(toplevelDomain);
    }

    const QString nameserver = data.value(NM_IODINE_KEY_NAMESERVER);
    if (!nameserver.isEmpty()) {
        m_ui->le_nameserver->setText(nameserver);
    }

    const NetworkManager::Setting::SecretFlags passwordFlag = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_IODINE_KEY_PASSWORD"-flags").toInt());
    if (passwordFlag == NetworkManager::Setting::None) {
        m_ui->le_password->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (passwordFlag == NetworkManager::Setting::AgentOwned) {
        m_ui->le_password->setPasswordOption(PasswordField::StoreForUser);
    } else {
        m_ui->le_password->setPasswordOption(PasswordField::AlwaysAsk);
    }

    const QString fragSize = data.value(NM_IODINE_KEY_FRAGSIZE);
    if (!fragSize.isEmpty()) {
        m_ui->sb_fragmentSize->setValue(fragSize.toInt());
    }

    loadSecrets(setting);
}

void IodineWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();

        const QString password = secrets.value(NM_IODINE_KEY_PASSWORD);
        if (!password.isEmpty()) {
            m_ui->le_password->setText(password);
        }
    }
}

QVariantMap IodineWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_IODINE));
    NMStringMap data;
    NMStringMap secrets;

    if (!m_ui->le_toplevelDomain->text().isEmpty()) {
        data.insert(NM_IODINE_KEY_TOPDOMAIN, m_ui->le_toplevelDomain->text());
    }

    if (!m_ui->le_nameserver->text().isEmpty()) {
        data.insert(NM_IODINE_KEY_NAMESERVER, m_ui->le_nameserver->text());
    }

    if (!m_ui->le_password->text().isEmpty()) {
        secrets.insert(NM_IODINE_KEY_PASSWORD, m_ui->le_password->text());
    }

    if (m_ui->le_password->passwordOption() == PasswordField::StoreForAllUsers) {
        data.insert(NM_IODINE_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::None));
    } else if (m_ui->le_password->passwordOption() == PasswordField::StoreForUser) {
        data.insert(NM_IODINE_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::AgentOwned));
    } else {
        data.insert(NM_IODINE_KEY_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotSaved));
    }

    if (m_ui->sb_fragmentSize->value()) {
        data.insert(NM_IODINE_KEY_FRAGSIZE, QString::number(m_ui->sb_fragmentSize->value()));
    }

    setting.setData(data);
    setting.setSecrets(secrets);
    return setting.toMap();
}

bool IodineWidget::isValid() const
{
    return !m_ui->le_toplevelDomain->text().isEmpty();
}
