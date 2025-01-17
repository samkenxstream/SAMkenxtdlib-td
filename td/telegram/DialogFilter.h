//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2023
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/DialogFilterId.h"
#include "td/telegram/DialogId.h"
#include "td/telegram/FolderId.h"
#include "td/telegram/InputDialogId.h"
#include "td/telegram/td_api.h"
#include "td/telegram/telegram_api.h"

#include "td/utils/common.h"
#include "td/utils/FlatHashMap.h"
#include "td/utils/Status.h"
#include "td/utils/StringBuilder.h"

#include <functional>

namespace td {

class Td;

class DialogFilter {
 public:
  static int32 get_max_filter_dialogs();

  static unique_ptr<DialogFilter> get_dialog_filter(telegram_api::object_ptr<telegram_api::DialogFilter> filter_ptr,
                                                    bool with_id);

  static Result<unique_ptr<DialogFilter>> create_dialog_filter(Td *td, DialogFilterId dialog_filter_id,
                                                               td_api::object_ptr<td_api::chatFilter> filter);

  void set_dialog_is_pinned(InputDialogId input_dialog_id, bool is_pinned);

  void set_pinned_dialog_ids(vector<InputDialogId> &&input_dialog_ids);

  void include_dialog(InputDialogId input_dialog_id);

  void remove_secret_chat_dialog_ids();

  void remove_dialog_id(DialogId dialog_id);

  bool is_empty(bool for_server) const;

  const DialogFilterId &get_dialog_filter_id() const {
    return dialog_filter_id_;
  }

  const vector<InputDialogId> &get_pinned_dialog_ids() const {
    return pinned_dialog_ids_;
  }

  bool is_dialog_pinned(DialogId dialog_id) const;

  bool is_dialog_included(DialogId dialog_id) const;

  bool can_include_dialog(DialogId dialog_id) const;

  Status check_limits() const;

  static string get_emoji_by_icon_name(const string &icon_name);

  string get_icon_name() const;

  static string get_default_icon_name(const td_api::chatFilter *filter);

  telegram_api::object_ptr<telegram_api::DialogFilter> get_input_dialog_filter() const;

  td_api::object_ptr<td_api::chatFilter> get_chat_filter_object(const vector<DialogId> &unknown_dialog_ids) const;

  td_api::object_ptr<td_api::chatFilterInfo> get_chat_filter_info_object() const;

  void for_each_dialog(std::function<void(const InputDialogId &)> callback) const;

  // merges changes from old_server_filter to new_server_filter in old_filter
  static unique_ptr<DialogFilter> merge_dialog_filter_changes(const DialogFilter *old_filter,
                                                              const DialogFilter *old_server_filter,
                                                              const DialogFilter *new_server_filter);

  void sort_input_dialog_ids(const Td *td, const char *source);

  vector<FolderId> get_folder_ids() const;

  bool can_have_archived_dialogs() const;

  bool need_dialog(const Td *td, DialogId dialog_id, bool has_unread_mentions, bool is_muted, bool has_unread_messages,
                   FolderId folder_id) const;

  static vector<DialogFilterId> get_dialog_filter_ids(const vector<unique_ptr<DialogFilter>> &dialog_filters,
                                                      int32 main_dialog_list_position);

  static bool set_dialog_filters_order(vector<unique_ptr<DialogFilter>> &dialog_filters,
                                       vector<DialogFilterId> dialog_filter_ids);

  static bool are_similar(const DialogFilter &lhs, const DialogFilter &rhs);

  static bool are_equivalent(const DialogFilter &lhs, const DialogFilter &rhs);

  static bool are_flags_equal(const DialogFilter &lhs, const DialogFilter &rhs);

  template <class StorerT>
  void store(StorerT &storer) const;

  template <class ParserT>
  void parse(ParserT &parser);

 private:
  DialogFilterId dialog_filter_id_;
  string title_;
  string emoji_;
  vector<InputDialogId> pinned_dialog_ids_;
  vector<InputDialogId> included_dialog_ids_;
  vector<InputDialogId> excluded_dialog_ids_;
  bool exclude_muted_ = false;
  bool exclude_read_ = false;
  bool exclude_archived_ = false;
  bool include_contacts_ = false;
  bool include_non_contacts_ = false;
  bool include_bots_ = false;
  bool include_groups_ = false;
  bool include_channels_ = false;

  static FlatHashMap<string, string> emoji_to_icon_name_;
  static FlatHashMap<string, string> icon_name_to_emoji_;

  static void init_icon_names();

  string get_chosen_or_default_icon_name() const;

  friend bool operator==(const DialogFilter &lhs, const DialogFilter &rhs);

  friend StringBuilder &operator<<(StringBuilder &string_builder, const DialogFilter &filter);
};

inline bool operator==(const DialogFilter &lhs, const DialogFilter &rhs) {
  return lhs.dialog_filter_id_ == rhs.dialog_filter_id_ && lhs.title_ == rhs.title_ && lhs.emoji_ == rhs.emoji_ &&
         lhs.pinned_dialog_ids_ == rhs.pinned_dialog_ids_ && lhs.included_dialog_ids_ == rhs.included_dialog_ids_ &&
         lhs.excluded_dialog_ids_ == rhs.excluded_dialog_ids_ && DialogFilter::are_flags_equal(lhs, rhs);
}

inline bool operator!=(const DialogFilter &lhs, const DialogFilter &rhs) {
  return !(lhs == rhs);
}

inline bool operator==(const unique_ptr<DialogFilter> &lhs, const unique_ptr<DialogFilter> &rhs) {
  return *lhs == *rhs;
}

inline bool operator!=(const unique_ptr<DialogFilter> &lhs, const unique_ptr<DialogFilter> &rhs) {
  return !(lhs == rhs);
}

StringBuilder &operator<<(StringBuilder &string_builder, const DialogFilter &filter);

}  // namespace td
