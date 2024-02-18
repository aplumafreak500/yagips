/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "item.h"
#include "define.pb.h"

typedef google::protobuf::uint32 uint32;

Item::Item(proto::Item pb) {
	proto::Material material;
	proto::MaterialDeleteInfo mdel;
	proto::MaterialDeleteInfo_DateTimeDelete dtdel;
	proto::Equip equip;
	proto::Reliquary artifact;
	proto::Weapon weapon;
	google::protobuf::Map<uint32, uint32> affixes;
	unsigned int i, j, k;
	unsigned short subStat;
	id = pb.item_id();
	guid = pb.guid();
	if (pb.has_material()) {
		material = pb.material();
		type = ITEM_MATERIAL;
		data.material.count = material.count();
		if (material.has_delete_info()) {
			mdel = material.delete_info();
			if (mdel.has_delete_config()) {
				// TODO: "countdown" (???) and "delay week countdown" (Transient Resin). These use a map-type in the proto so that individually obtained instances of the item can expire without the whole stack expiring.
				if (mdel.has_date_delete()) {
					dtdel = mdel.date_delete();
					data.material.expireTime = dtdel.delete_time();
				}
			}
		}
	}
	else if (pb.has_equip()) {
		equip = pb.equip();
		if (equip.has_reliquary()) {
			type = ITEM_ARTIFACT;
			data.artifact.locked = equip.is_locked();
			artifact = equip.reliquary();
			data.artifact.level = artifact.level();
			data.artifact.exp = artifact.exp();
			data.artifact.mainStat = artifact.main_prop_id();
			k = 0;
			for (i = 0; i < (unsigned int) artifact.append_prop_id_list_size(); i++) {
				subStat = artifact.append_prop_id_list(i);
				for (j = 0; j < k; j++) {
					if (data.artifact.subStats[j].id == subStat) {
						data.artifact.subStats[j].count++;
						subStat = 0;
						break;
					}
				}
				if (subStat) {
					data.artifact.subStats[k].id = subStat;
					data.artifact.subStats[k].count = 1;
					k++;
				}
			}
			data.artifact.numSubStats = k;
		}
		else if (equip.has_weapon()) {
			type = ITEM_WEAPON;
			data.weapon.locked = equip.is_locked();
			weapon = equip.weapon();
			data.weapon.level = weapon.level();
			data.weapon.exp = weapon.exp();
			data.weapon.ascension = weapon.promote_level();
			affixes = weapon.affix_map();
			i = 0;
			k = 0;
			for (auto aid : affixes) {
				data.weapon.affixes[i] = aid.first;
				k = MAX(k, aid.second);
				i++;
			}
			data.weapon.numAffixes = i;
			data.weapon.refinement = k;
		}
	}
}

Item::operator proto::Item() const {
	proto::Item ret;
	proto::Material* material;
	proto::MaterialDeleteInfo* mdel;
	proto::MaterialDeleteInfo_DateTimeDelete* dtdel;
	proto::Equip* equip;
	proto::Reliquary* artifact;
	proto::Weapon* weapon;
	google::protobuf::Map<uint32, uint32>* affixes;
	unsigned int i, j;
	ret.set_item_id(id);
	ret.set_guid(guid);
	switch (type) {
	default:
		return ret;
	case ITEM_MATERIAL:
		material = new proto::Material();
		material->set_count(data.material.count);
		// TODO: "countdown" (???) and "delay week countdown" (Transient Resin). These use a map-type in the proto so that individually obtained instances of the item can expire without the whole stack expiring.
		if (data.material.expireTime != 0) {
			mdel = new proto::MaterialDeleteInfo();
			dtdel = new proto::MaterialDeleteInfo_DateTimeDelete();
			dtdel->set_delete_time(data.material.expireTime);
			mdel->set_allocated_date_delete(dtdel);
			mdel->set_has_delete_config(1);
			material->set_allocated_delete_info(mdel);
		}
		ret.set_allocated_material(material);
		return ret;
	case ITEM_ARTIFACT:
		equip = new proto::Equip();
		equip->set_is_locked(data.artifact.locked);
		artifact = new proto::Reliquary();
		artifact->set_level(data.artifact.level);
		artifact->set_exp(data.artifact.exp);
		artifact->set_promote_level(data.artifact.level / 4); // TODO is this correct?
		artifact->set_main_prop_id(data.artifact.mainStat);
		for (i = 0; i < data.artifact.numSubStats; i++) {
			for (j = 0; j < data.artifact.subStats[i].count; j++) {
				artifact->add_append_prop_id_list(data.artifact.subStats[i].id);
			}
		}
		equip->set_allocated_reliquary(artifact);
		ret.set_allocated_equip(equip);
		return ret;
	case ITEM_WEAPON:
		equip = new proto::Equip();
		equip->set_is_locked(data.weapon.locked);
		weapon = new proto::Weapon();
		weapon->set_level(data.weapon.level);
		weapon->set_exp(data.weapon.exp);
		weapon->set_promote_level(data.weapon.ascension);
		affixes = weapon->mutable_affix_map();
		for (i = 0; i < data.weapon.numAffixes; i++) {
			(*affixes)[data.weapon.affixes[i]] = data.weapon.refinement;
		}
		equip->set_allocated_weapon(weapon);
		ret.set_allocated_equip(equip);
		return ret;
	}
}
