/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <fstream>
#include <iostream>

#include "repos_parser.hpp"
#include "src/manager/commands/command_init/constants/command_init_constants.hpp"

void ReposParser::Parse(std::string const & reposPath)
{
  std::fstream reposFile;
  reposFile.open(reposPath, std::ios::in);
  if (reposFile.is_open())
  {
    std::string line;
    bool is_repos = false;
    bool is_components = false;

    while (std::getline(reposFile, line))
    {
      if (!line.empty())
      {
        if (line.find(SpecificationConstants::REPOS_SECTION_HEADER) != std::string::npos)
        {
          is_repos = true;
          is_components = false;
          continue;
        }

        if (line.find(SpecificationConstants::COMPONENTS_SECTION_HEADER) != std::string::npos)
        {
          is_components = true;
          is_repos = false;
          continue;
        }

        if (is_repos)
          m_repositories.push_back(line);
        if (is_components)
          m_components.push_back(line);
      }
    }
    reposFile.close();
  }
}

std::vector<std::string> ReposParser::GetComponents()
{
  return m_components;
}

std::vector<std::string> ReposParser::GetRepositories()
{
  return m_repositories;
}
