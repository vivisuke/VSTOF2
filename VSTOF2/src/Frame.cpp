#include "Frame.h"

void Frame::reset() {
	//変数の初期化
	root = nullptr; //rootフレームのポインタ
	parent = nullptr; //親フレームのポインタ
	index = 0;//同フレーム内の自フレームの割当番号(=0,1,2,3,...)
	pos = { 0,0,0,0 }; //フレーム座標
	size = { 0, 0 }; //フレームサイズ(末端フレームのみ代入)
	name = ""; //フレームの名称
	description = ""; //フレーム内のUIの解説
	mode = 0; //子フレームが縦並び=0,横並び=1
	gap = 0; //子フレーム間同士の隙間(px単位)
	length = 0; //全フレームが初期値サイズ時の自フレームのサイズ
	lock = 0; //各子フレームの長さ(mode=0なら縦幅,mode=1なら横幅)の固定on/off
	lock_length = 0; //固定サイズの全子フレームと全gapの和(末端フレームは0を代入)
	return;
}

void Frame::main_draw() {
	return;
}

void Frame::childs_draw() {
	for (int i = 0; i < childs.size(); i++) {
		childs[i]->draw();
	}
	return;
}

void Frame::when_create() {
	//自フレームのインデックス番号取得
	get_index();
	//自フレームのツリーのrootフレーム取得
	get_root();
	//rootフレーム以下のlength等再算出
	root->get_length();
	//rootフレーム以下のフレーム再配置
	root->resize();
	//WINDOW_INFOを親フレームと同一のものにする
	if (parent != nullptr) {
		win = parent->win;
	}
}

void Frame::set_win_info(WINDOW_INFO *set_win) {
	if ((root->win != set_win) && (this != root)) {
		root->set_win_info(set_win);
	}
	else {
		win = set_win;
		for (int i = 0; i < childs.size(); i++) {
			childs[i]->set_win_info(set_win);
		}
	}
}

void Frame::resize(){
	size.x = pos.right - pos.left;
	size.y = pos.bottom - pos.top;
	if (childs.size() != 0) {
		if (mode) {
			//横並びの場合
			//子フレーム分ループ
			for (int i = 0; i < childs.size(); i++) {
				//i個目の子フレーム位置算出
				//上下位置
				childs[i]->pos.top = pos.top + gap;
				childs[i]->pos.bottom = pos.bottom - gap;
				//左位置
				if (i == 0) {
					childs[i]->pos.left = pos.left + gap;
				}
				else {
					childs[i]->pos.left = childs[i - 1]->pos.right + gap;
				}
				//右位置
				if (childs[i]->lock) {
					//固定サイズフレームの場合は割合変換不要
					childs[i]->pos.right =
						childs[i]->pos.left +
						childs[i]->length;
				}
				else {
					//非固定サイズフレームの場合は割合計算
					childs[i]->pos.right =
						childs[i]->pos.left +
						Utility::Proportion(
							childs[i]->length,
							0,
							length - lock_length,
							0,
							size.x - lock_length
						);
				}
				//子フレーム位置設定
				childs[i]->resize();
			}
		}
		else {
			//縦並びの場合
			//子フレーム分ループ
			for (int i = 0; i < childs.size(); i++) {
				//i個目の子フレーム位置算出
				//左右位置
				childs[i]->pos.left = pos.left + gap;
				childs[i]->pos.right = pos.right - gap;
				//上位置
				if (i == 0) {
					childs[i]->pos.top = pos.top + gap;
				}
				else {
					childs[i]->pos.top = childs[i - 1]->pos.bottom + gap;
				}
				//下位置
				if (childs[i]->lock) {
					//固定サイズフレームorスクロール可フレームの場合は割合変換不要
					childs[i]->pos.bottom =
						childs[i]->pos.top +
						childs[i]->length;
				}
				else {
					//非固定サイズフレームの場合は割合計算
					childs[i]->pos.bottom =
						childs[i]->pos.top +
						Utility::Proportion(
							childs[i]->length,
							0,
							length - lock_length,
							0,
							size.y - lock_length
						);
				}
				//子フレーム位置設定
				childs[i]->resize();
			}
		}
	}
	return;
}

void Frame::draw() {
	main_draw();
	childs_draw();
	return;
}

void Frame::get_length() {
	if (childs.size() != 0) {
		length = 0; //全子フレームと全gapの和
		lock_length = 0; //固定サイズの全子フレームと全gapの和(末端フレームは0を代入)
		//子フレームの個数分ループ
		for (int i = 0; i < childs.size(); i++) {
			//子フレームが子フレームを持っていた場合、再帰
			if (childs.size() != 0) {
				childs[i]->get_length();
			}
			//親フレームの長さに子フレームの長さを足していく
			length += childs[i]->length;
			//もし子フレームが末端フレームで、固定サイズフレームの場合
			if (childs[i]->lock) {
				lock_length += childs[i]->length;
			}
		}
		//全gapを足す
		length += gap * (childs.size() + 1);
		lock_length += gap * (childs.size() + 1);
	}
	return;
}

void Frame::get_root() {
	if (parent != nullptr) {
		parent->get_root();
		root = parent->root;
	}
	else {
		root = this;
	}
return;
}

void Frame::get_index() {
	if (parent != nullptr) {
		index = parent->childs.size() - 1;
	}
	else {
		index = 0;
	}
}

Frame::Frame(Frame *set_parent = nullptr, int set_length = 0, bool set_lock = 0, std::string set_name = "", std::string set_description = "") {
	reset();
	parent = set_parent;
	length = set_length;
	lock = set_lock;
	name = set_name;
	description = set_description;
	when_create();
}
Frame::Frame(bool set_mode, Frame *set_parent, std::string set_name = "", std::string set_description = "") {
	reset();
	mode = set_mode;
	parent = set_parent;
	name = set_name;
	description = set_description;
	when_create();
}