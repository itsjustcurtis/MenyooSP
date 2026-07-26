#pragma once
#include <string>

namespace MenuCategory
{
	void ResetCategoryState();
	bool AddCategory(const std::string& label, bool defaultExpanded = true);
	void ExpandAll();
	void RestoreExpandedState();
	bool AddCategoryOption(const std::string& text, bool& pressed);
	void JumpToAdjacentCategory(const std::string& currentCategory);
	bool HasMultipleCategories();
}