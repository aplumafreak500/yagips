/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "account.h"
#include "dbgate.h"
#include "player.h"

Player::Player() {}
Player::~Player() {}

const Account* Player::getAccount() const {
	return account;
}

void Player::setAccount(const Account* a) {
	account = a;
}

unsigned long Player::getUid() const {
	return uid;
}

void Player::setUid(unsigned long u) {
	uid = u;
}

int Player::saveToDb() const {
	return globalDbGate->savePlayer(*this);
}
