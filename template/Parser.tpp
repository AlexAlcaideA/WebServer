#include <vector>
#include <string>

template <typename Context>
bool SetRoot(Context* ctx, const std::vector<std::string>& args)
{
	if (args.size() != 1)
		return false;
	if (ctx->root != NULL)
		return false;
	ctx->root = new std::string(args[0]);
	return true;
}

template <typename Context>
bool AddIndex(Context* ctx, const std::vector<std::string>& args)
{
	if (args.size() != 1)
		return false;
	if (ctx->index != NULL)
		return false;
	ctx->index = new std::string(args[0]);
	return true;
}
