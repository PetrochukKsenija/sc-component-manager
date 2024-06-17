/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "commands/sc_component_manager_command.hpp"

class ScComponentManagerCommandSearch : public ScComponentManagerCommand
{
public:
  ScComponentManagerCommandSearch() = default;

  ScAddrUnorderedSet Execute(ScMemoryContext * context, ScAddr const & actionAddr) override;

protected:
  std::string const SPECIFICATION_ALIAS = "_specification";
  std::string const COMPONENT_ALIAS = "_component";
  std::string const AUTHORS_SET_ALIAS = "_authors";
  std::string const EXPLANATION_LINK_ALIAS = "_explanation";
  std::string const NOTE_LINK_ALIAS = "_note";
  std::string const PURPOSE_LINK_ALIAS = "_purpose";
  std::string const KEY_SET_ALIAS = "_key";
  std::string const ID_LINK_ALIAS = "_id";

  std::string const AUTHOR = "author";
  std::string const CLASS = "class";
  std::string const EXPLANATION = "explanation";
  std::string const NOTE = "note";
  std::string const PURPOSE = "purpose";
  std::string const KEY = "key";
  std::string const MAIN_ID = "main-id";
  std::set<std::string> const possibleSearchParameters = {AUTHOR, CLASS, EXPLANATION, NOTE, PURPOSE, KEY, MAIN_ID}; // extend to more params

  void SearchComponentsByClass(
      ScMemoryContext * context,
      ScTemplate & searchComponentTemplate,
      std::vector<std::string> const & parameters);

  void SearchComponentsByRelationSet(
      ScMemoryContext * context,
      ScAddr const & relationAddr,
      std::string const & setAlias,
      ScTemplate & searchComponentTemplate,
      std::vector<std::string> const & parameters);

  ScAddrUnorderedSet SearchComponentsByRelationLink(
      ScMemoryContext * context,
      ScAddr const & relationAddr,
      std::string const & linkAlias,
      ScTemplate & searchComponentTemplate,
      std::vector<std::string> const & parameters);

  ScAddrUnorderedSet SearchComponentsSpecifications(
      ScMemoryContext * context,
      ScTemplate & searchComponentTemplate,
      std::map<std::string, ScAddrUnorderedSet> const & linksValues);

  ScAddrUnorderedSet SearchComponentsSpecificationsWithoutLinks(
      ScMemoryContext * context,
      ScTemplateSearchResult const & searchComponentResult);

  ScAddrUnorderedSet SearchComponentsSpecificationsWithLinks(
      ScMemoryContext * context,
      ScTemplateSearchResult const & searchComponentResult,
      std::map<std::string, ScAddrUnorderedSet> const & linksValues);
};
