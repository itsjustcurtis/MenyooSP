#pragma once
#include <string>
#include <vector>

namespace MenuCategory
{
	void ResetCategoryState();
	bool AddCategory(const std::string& label, bool defaultExpanded = true);
	void ExpandAll();
	void RestoreExpandedState();

	const std::vector<std::string>& GetCategoryLabels();
	const std::vector<int>& GetCategoryPositions();
	void JumpToCategory(size_t index);
}
