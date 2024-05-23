/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "common_utils.hpp"
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "sc-memory/sc_memory.hpp"

#include "constants/command_constants.hpp"
#include "module/keynodes/ScComponentManagerKeynodes.hpp"

namespace common_utils
{
std::map<std::string, ScAddr> CommonUtils::managerParametersWithAgentRelations;
std::vector<std::vector<ScAddr>> CommonUtils::componentsClasses;

ScAddr CommonUtils::GetMyselfDecompositionAddr(ScMemoryContext & context)
{
  return utils::IteratorUtils::getAnyByOutRelation(
      &context,
      keynodes::ScComponentManagerKeynodes::myself,
      keynodes::ScComponentManagerKeynodes::nrel_ostis_system_decomposition);
}

void CommonUtils::CreateMyselfDecomposition(ScMemoryContext & context)
{
  ScAddr myselfDecompositionAddr = context.CreateNode(ScType::NodeConst);
  utils::GenerationUtils::generateRelationBetween(
      &context,
      keynodes::ScComponentManagerKeynodes::myself,
      myselfDecompositionAddr,
      keynodes::ScComponentManagerKeynodes::nrel_ostis_system_decomposition);
  ScAddr componentAddr;
  ScAddr componentClassAddr;
  ScAddr componentDecompositionAddr;

  for (ScAddrVector const & subsystemAndComponentClass : componentsClasses)
  {
    componentClassAddr = subsystemAndComponentClass[1];
    componentAddr = context.CreateNode(ScType::NodeConst);

    context.CreateEdge(ScType::EdgeAccessConstPosPerm, componentClassAddr, componentAddr);
    context.CreateEdge(ScType::EdgeAccessConstPosPerm, myselfDecompositionAddr, componentAddr);

    componentDecompositionAddr = context.CreateNode(ScType::NodeConst);
    utils::GenerationUtils::generateRelationBetween(
        &context, componentAddr, componentDecompositionAddr, keynodes::ScComponentManagerKeynodes::nrel_decomposition);
  }
}

void CommonUtils::InitParametersMap()
{
  managerParametersWithAgentRelations = {
      {"author", keynodes::ScComponentManagerKeynodes::rrel_author},
      {"class", keynodes::ScComponentManagerKeynodes::rrel_class},
      {"explanation", keynodes::ScComponentManagerKeynodes::rrel_explanation},
      {"note", keynodes::ScComponentManagerKeynodes::rrel_note},
      {"purpose", keynodes::ScComponentManagerKeynodes::rrel_purpose},
      {"key", keynodes::ScComponentManagerKeynodes::rrel_key_sc_element},
      {"id", keynodes::ScComponentManagerKeynodes::rrel_main_idtf},
      {"idtf", scAgentsCommon::CoreKeynodes::rrel_1},
      {"search", keynodes::ScComponentManagerKeynodes::action_components_search},
      {"install", keynodes::ScComponentManagerKeynodes::action_components_install},
      {"init", keynodes::ScComponentManagerKeynodes::action_components_init}};

  componentsClasses = {
      {keynodes::ScComponentManagerKeynodes::concept_reusable_ui_component,
       keynodes::ScComponentManagerKeynodes::sc_model_of_interface},
      {keynodes::ScComponentManagerKeynodes::concept_reusable_ps_component,
       keynodes::ScComponentManagerKeynodes::sc_model_of_problem_solver},
      {keynodes::ScComponentManagerKeynodes::concept_reusable_kb_component,
       keynodes::ScComponentManagerKeynodes::sc_model_of_knowledge_base},
      {keynodes::ScComponentManagerKeynodes::concept_reusable_embedded_ostis_system,
       keynodes::ScComponentManagerKeynodes::concept_subsystems_set}};
}

bool CommonUtils::TransformToScStruct(
    ScMemoryContext & context,
    ScAddr const & actionAddr,
    std::map<std::string, std::vector<std::string>> const & commandParameters)
{
  ScAddr parameterValueAddr;
  ScAddr parameterRrelNodeAddr;
  ScAddr edgeAddr;
  ScAddr setAddr;
  for (auto const & parameter : commandParameters)
  {
    try
    {
      parameterRrelNodeAddr = managerParametersWithAgentRelations.at(parameter.first);
    }
    catch (std::out_of_range const & exception)
    {
      SC_LOG_INFO("Transform to sc-structure: Unknown parameter " << parameter.first);
      continue;
    }
    setAddr = context.CreateNode(ScType::NodeConst);
    utils::GenerationUtils::generateRelationBetween(&context, actionAddr, setAddr, parameterRrelNodeAddr);

    for (std::string const & parameterValue : parameter.second)
    {
      if (parameter.first == CommandsConstantsFlags::EXPLANATION)
      {
        parameterValueAddr = context.CreateNode(ScType::LinkConst);
        context.SetLinkContent(parameterValueAddr, parameterValue);
      }
      else if (parameter.first == CommandsConstantsFlags::NOTE)
      {
        parameterValueAddr = context.CreateNode(ScType::LinkConst);
        context.SetLinkContent(parameterValueAddr, parameterValue);
      }
      else if (parameter.first == CommandsConstantsFlags::PURPOSE)
      {
        parameterValueAddr = context.CreateNode(ScType::LinkConst);
        context.SetLinkContent(parameterValueAddr, parameterValue);
      }
      else if (parameter.first == CommandsConstantsFlags::ID)
      {
        parameterValueAddr = context.CreateNode(ScType::LinkConst);
        context.SetLinkContent(parameterValueAddr, parameterValue);
      }
      else
      {
        parameterValueAddr = context.HelperFindBySystemIdtf(parameterValue);
        if (!context.IsElement(parameterValueAddr))
        {
          SC_LOG_WARNING("Transform to sc-structure: Unknown value: " << parameterValue);
          parameterValueAddr = context.CreateNode(ScType::NodeConst);
          context.HelperSetSystemIdtf(parameterValue, parameterValueAddr);
        }
      }

      context.CreateEdge(ScType::EdgeAccessConstPosPerm, setAddr, parameterValueAddr);
    }
  }
  return true;
}

ScAddrVector CommonUtils::GetNodesUnderParameter(
    ScMemoryContext & context,
    ScAddr const & actionAddr,
    ScAddr const & relationAddr)
{
  ScAddr parameterNode;
  ScAddrVector components;
  ScIterator5Ptr const & parameterIterator = context.Iterator5(
      actionAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, relationAddr);
  if (parameterIterator->Next())
  {
    parameterNode = parameterIterator->Get(2);
    ScIterator3Ptr const & componentsIterator =
        context.Iterator3(parameterNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
    while (componentsIterator->Next())
    {
      if (context.GetElementType(componentsIterator->Get(2)) == ScType::NodeConstClass)
      {
        ScIterator3Ptr const & elementsIterator =
            context.Iterator3(componentsIterator->Get(2), ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
        while (elementsIterator->Next())
        {
          components.push_back(elementsIterator->Get(2));
        }
      }
      else
      {
        components.push_back(componentsIterator->Get(2));
      }
    }
  }
  return components;
}

std::map<std::string, std::vector<std::string>> CommonUtils::GetCommandParameters(
    ScMemoryContext & context,
    ScAddr const & actionAddr)
{
  std::map<std::string, std::vector<std::string>> commandParameters;

  ScAddr const & authorsSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_author);
  std::map<std::string, ScAddr> const & authors = GetSetElements(context, authorsSetAddr);

  ScAddr const & classesSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_class);
  std::map<std::string, ScAddr> const & classes = GetSetElements(context, classesSetAddr);

  ScAddr const & explanationsSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_explanation);
  std::map<std::string, ScAddr> const & explanations = GetElementsLinksOfSet(context, explanationsSetAddr);

  ScAddr const & notesSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_note);
  std::map<std::string, ScAddr> const & notes = GetElementsLinksOfSet(context, notesSetAddr);

  ScAddr const & purposesSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_purpose);
  std::map<std::string, ScAddr> const & purposes = GetElementsLinksOfSet(context, purposesSetAddr);

  ScAddr const & keysSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_key_sc_element);
  std::map<std::string, ScAddr> const & keys = GetElementsLinksOfSet(context, keysSetAddr);

  ScAddr const & idsSetAddr =
      GetParameterNodeUnderRelation(context, actionAddr, keynodes::ScComponentManagerKeynodes::rrel_main_idtf);
  std::map<std::string, ScAddr> const & ids = GetElementsLinksOfSet(context, idsSetAddr);

  std::vector<std::string> authorsList, classesList, explanationsList, notesList, purposesList, keysList, idsList;

  if (context.IsElement(authorsSetAddr))
  {
    for (auto & el : authors)
      authorsList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::AUTHOR, authorsList});
  }
  if (context.IsElement(classesSetAddr))
  {
    for (auto & el : classes)
      classesList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::CLASS, classesList});
  }
  if (context.IsElement(explanationsSetAddr))
  {
    for (auto & el : explanations)
      explanationsList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::EXPLANATION, explanationsList});
  }
  if (context.IsElement(notesSetAddr))
  {
    for (auto & el : notes)
      notesList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::NOTE, notesList});
  }
  if (context.IsElement(purposesSetAddr))
  {
    for (auto & el : purposes)
      purposesList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::PURPOSE, purposesList});
  }
  if (context.IsElement(keysSetAddr))
  {
    for (auto & el : keys)
      keysList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::KEY, keysList});
  }
  if (context.IsElement(idsSetAddr))
  {
    for (auto & el : ids)
      idsList.push_back(el.first);
    commandParameters.insert({CommandsConstantsFlags::ID, idsList});
  }
  return commandParameters;
}

ScAddr CommonUtils::GetParameterNodeUnderRelation(
    ScMemoryContext & context,
    ScAddr const & actionAddr,
    ScAddr const & relation)
{
  ScAddr parameterNode;
  ScIterator5Ptr const & parameterIterator = context.Iterator5(
      actionAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, relation);
  if (parameterIterator->Next())
  {
    parameterNode = parameterIterator->Get(2);
  }
  return parameterNode;
}

std::map<std::string, ScAddr> CommonUtils::GetSetElements(ScMemoryContext & context, ScAddr const & setAddr)
{
  std::map<std::string, ScAddr> elementsIdtfAndAddr;
  if (!context.IsElement(setAddr))
    return elementsIdtfAndAddr;
  ScIterator3Ptr const & elementsIterator =
      context.Iterator3(setAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while (elementsIterator->Next())
  {
    try
    {
      elementsIdtfAndAddr.insert({context.HelperGetSystemIdtf(elementsIterator->Get(2)), elementsIterator->Get(2)});
    }
    catch (std::exception const & exception)
    {
      SC_LOG_WARNING("CommonUtils::GetSetElements : met element without system identifier");
      continue;
    }
  }
  return elementsIdtfAndAddr;
}

std::map<std::string, ScAddr> CommonUtils::GetElementsLinksOfSet(ScMemoryContext & context, ScAddr const & setAddr)
{
  std::map<std::string, ScAddr> elementsIdtfAndAddr;
  if (!context.IsElement(setAddr))
    return elementsIdtfAndAddr;
  std::string elementIdtf;
  ScIterator3Ptr const & elementsIterator =
      context.Iterator3(setAddr, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
  while (elementsIterator->Next())
  {
    context.GetLinkContent(elementsIterator->Get(2), elementIdtf);
    elementsIdtfAndAddr.insert({elementIdtf, elementsIterator->Get(2)});
  }
  return elementsIdtfAndAddr;
}

ScAddr CommonUtils::GetSubsystemDecompositionAddr(ScMemoryContext & context, ScAddr const & component)
{
  ScAddr componentDecomposition;
  ScAddr componentClass;
  if (!context.IsElement(component))
  {
    return componentDecomposition;
  }

  for (std::vector<ScAddr> const & commonComponentClass : common_utils::CommonUtils::componentsClasses)
  {
    if (context.HelperCheckEdge(commonComponentClass[0], component, ScType::EdgeAccessConstPosPerm))
    {
      componentClass = commonComponentClass[1];
      break;
    }
  }
  ScAddr myselfDecomposition = utils::IteratorUtils::getAnyByOutRelation(
      &context,
      keynodes::ScComponentManagerKeynodes::myself,
      keynodes::ScComponentManagerKeynodes::nrel_ostis_system_decomposition);

  if (!context.IsElement(myselfDecomposition))
    return componentDecomposition;

  ScIterator3Ptr partsDecomposition =
      context.Iterator3(myselfDecomposition, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

  while (partsDecomposition->Next())
  {
    if (!context.HelperCheckEdge(componentClass, partsDecomposition->Get(2), ScType::EdgeAccessConstPosPerm))
      continue;

    componentDecomposition = utils::IteratorUtils::getAnyByOutRelation(
        &context, partsDecomposition->Get(2), keynodes::ScComponentManagerKeynodes::nrel_decomposition);
    break;
  }
  return componentDecomposition;
}

bool CommonUtils::CheckIfInstalled(ScMemoryContext & context, ScAddr const & component)
{
  ScAddr decompositionAddr = GetSubsystemDecompositionAddr(context, component);
  if (!context.IsElement(decompositionAddr) || !context.IsElement(component))
    return false;
  return context.HelperCheckEdge(decompositionAddr, component, ScType::EdgeAccessConstPosPerm);
}

ScAddr CommonUtils::GetComponentBySpecification(ScMemoryContext & context, ScAddr const & specification)
{
  return utils::IteratorUtils::getAnyByOutRelation(
      &context, specification, scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
}

}  // namespace common_utils
