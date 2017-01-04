/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include <functional>
#include <vector>
#include "params.h"

namespace wl {

class base_inventory final {
public:
	using funcT = std::function<LONG_PTR(params)>; // works for both LRESULT and LONG_PTR

private:
	template<typename idT>
	class _depot final {
    private:
        std::vector<std::pair<idT, funcT>> _msgUnits;
    public:
        _depot() {
			this->_msgUnits.reserve(21); // arbitrary, to save realloc time
			this->_msgUnits.emplace_back(); // 1st element is sentinel room
		}

		funcT* add(idT id, funcT func) {
			this->_msgUnits.emplace_back(id, std::move(func)); // reverse search: messages can be overwritten
			return &this->_msgUnits.back().second;
		}

        funcT* find(idT id) {
			this->_msgUnits[0].first = id; // sentinel for reverse linear search
			std::pair<idT, funcT>* revRunner = &this->_msgUnits.back();
			while (revRunner->first != id) --revRunner;
			return revRunner == &this->_msgUnits[0] ?
				nullptr : &revRunner->second;
		}
	};

	_depot<UINT> _messages;
	_depot<WORD> _commands;
	_depot<std::pair<UINT_PTR, UINT>> _notifies;

public:
	funcT* find_func(params p) {
		funcT* func = nullptr;
		if (p.message == WM_COMMAND) { // if user adds raw WM_COMMAND or WM_NOTIFY, they will never be called
			return this->_commands.find(LOWORD(p.wParam));
		} else if (p.message == WM_NOTIFY) {
			return this->_notifies.find({
				reinterpret_cast<NMHDR*>(p.lParam)->idFrom,
				reinterpret_cast<NMHDR*>(p.lParam)->code
			});
		}
		return this->_messages.find(p.message);
	}

	void add_message(UINT msg, funcT func)                    { this->_messages.add(msg, std::move(func)); }
	void add_command(WORD cmd, funcT func)                    { this->_commands.add(cmd, std::move(func)); }
	void add_notify(UINT_PTR idFrom, UINT code, funcT func)   { this->_notifies.add({idFrom, code}, std::move(func));	}
	void add_notify(std::pair<UINT_PTR, UINT> id, funcT func) { this->_notifies.add({id.first, id.second}, std::move(func));	}

	void add_message(std::initializer_list<UINT> msgs, funcT func) {
		UINT firstMsg = *msgs.begin();
		funcT* pFirstFunc = this->_messages.add(firstMsg, std::move(func)); // store user func once
		for (size_t i = 1; i < msgs.size(); ++i) {
			if (*(msgs.begin() + i) != firstMsg) { // avoid overwriting
				this->add_message(*(msgs.begin() + i), [firstMsg, pFirstFunc](params p)->LONG_PTR {
                    p.message = firstMsg;
					return (*pFirstFunc)(p); // store light wrapper to 1st func
				});
			}
		}
	}

    void add_command(std::initializer_list<WORD> cmds, funcT func) {
		WORD firstCmd = *cmds.begin();
		funcT* pFirstFunc = this->_commands.add(firstCmd, std::move(func));
		for (size_t i = 1; i < cmds.size(); ++i) {
			if (*(cmds.begin() + i) != firstCmd) {
				this->add_command(*(cmds.begin() + i), [firstCmd, pFirstFunc](params p)->LONG_PTR {
					p.wParam = MAKEWPARAM(firstCmd, HIWORD(p.wParam));
					return (*pFirstFunc)(p);
				});
			}
		}
	}

    void add_notify(std::initializer_list<std::pair<UINT_PTR, UINT>> ids, funcT func) {
		std::pair<UINT_PTR, UINT> firstId = *ids.begin();
		funcT* pFirstFunc = this->_notifies.add({firstId.first, firstId.second}, std::move(func));
		for (size_t i = 1; i < ids.size(); ++i) {
			if (*(ids.begin() + i) != firstId) {
				this->add_notify(*(ids.begin() + i), [firstId, pFirstFunc](params p)->LONG_PTR {
					reinterpret_cast<NMHDR*>(p.lParam)->idFrom = firstId.first;
					reinterpret_cast<NMHDR*>(p.lParam)->code = firstId.second;
                    return (*pFirstFunc)(p);
				});
			}
		}
	}
};

}//namespace wl