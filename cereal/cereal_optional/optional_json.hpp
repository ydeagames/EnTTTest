#ifndef CEREAL_OPTIONAL_OPTIONAL_JSON_HPP
#define CEREAL_OPTIONAL_OPTIONAL_JSON_HPP

#include "cereal/archives/json.hpp"
#include "optional_nvp.hpp"

namespace cereal {

template<class T, class TV>
void prologue(JSONInputArchive &, const OptionalNameValuePair<T, TV> &) { }

template<class T, class TV>
void epilogue(JSONInputArchive &, const OptionalNameValuePair<T, TV> &) { }

template<class T, class TV>
void prologue(JSONOutputArchive &, const OptionalNameValuePair<T, TV> &) { }

template<class T, class TV>
void epilogue(JSONOutputArchive &, const OptionalNameValuePair<T, TV> &) { }

template<typename TV, typename T>
auto loadDefault(int, T& nvp) ->std::enable_if_t<!std::is_same<TV, void>::value>
{
	nvp.value = std::move(nvp.defaultValue);
}

template<typename TV, typename T>
void loadDefault(bool, T& nvp)
{
}

/// Loads optional nvp from JSONInputArchive if any. In case of exception sets fallback value
/// @param archive JSONInputArchive
/// @param nvp Optional NVP to load
template<class T, class TV>
void CEREAL_LOAD_FUNCTION_NAME(JSONInputArchive &archive, OptionalNameValuePair<T, TV> &nvp) {
    if (archive.hasName(nvp.name))
	{
        archive.setNextName(nvp.name);
        archive(nvp.value);
    }
	else
	{
        //if constexpr(!std::is_same<TV, void>::value) {
        //nvp.value = std::move(nvp.defaultValue);
		loadDefault<TV>(0, nvp);
        //}
        archive.setNextName(nullptr);
    }
}

/// Saves optional nvp to JSONOutputArchive in standard way like NameValuePair
/// @param archive JSONOutputArchive
/// @param nvp Optional NVP to save
template<class T, class TV>
void CEREAL_SAVE_FUNCTION_NAME(JSONOutputArchive &archive, OptionalNameValuePair<T, TV> const &nvp) {
    archive.setNextName(nvp.name);
    archive(nvp.value);
}

}

#endif //CEREAL_OPTIONAL_OPTIONAL_JSON_HPP
