#pragma once

class GameContext;

namespace ECS
{
	class IdentifierResolver
	{
	private:
		template<typename T, typename = decltype(&T::Identifier)>
		static const char* name0(int)
		{
			return T::Identifier;
		}

		template<typename T>
		static const char* name0(bool)
		{
			const char* name = typeid(T).name();
			auto ptr = std::strchr(name, ' ');
			if (ptr == nullptr)
				return name;
			return ptr + 1;
		}

	public:
		template<typename T>
		static const char* name()
		{
			return name0<T>(0);
		}
	};

	template <typename Entity>
	class ObjectSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				cereal::size_type size;
				archive.loadSize(size);
				entity = stack = static_cast<Entity>(size);
			}
			else
			{
				stack--;
				archive(entity);
			}
		}
	};

	template <typename Entity>
	class ObjectTagSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectTagSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				entity = stack = 1;
			}
			else
			{
				assert(false);
			}
		}

		template<typename T>
		void operator()(Entity& entity, T& tag)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("tag", tag));
				archive.finishNode();
			}
		}

	private:
		template<typename Component>
		void tag0(entt::SnapshotLoader<Entity>& loader) {
			const char* name = IdentifierResolver::name<Component>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				loader.tag<Component>(*this);
			}
		}

	public:
		template<typename... Component>
		void tag(entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Component>(loader), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectComponentSnapshotLoader
	{
	private:
		cereal::JSONInputArchive& archive;
		Entity stack = 0;

	public:
		ObjectComponentSnapshotLoader(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity& entity)
		{
			if (stack == 0)
			{
				archive.startNode();
				cereal::size_type size;
				archive.loadSize(size);
				entity = stack = static_cast<Entity>(size);
			}
			else
			{
				assert(false);
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

		template<typename T>
		void operator()(Entity& entity, T& component)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("component", component));
				archive.finishNode();
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

	private:
		template<typename Component>
		void component0(entt::SnapshotLoader<Entity>& loader) {
			const char* name = IdentifierResolver::name<Component>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				loader.component<Component>(*this);
			}
		}

	public:
		template<typename... Component>
		void component(entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Component>(loader), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				stack = entity;
			}
			else
			{
				stack--;
				archive(entity);
			}
		}
	};

	template <typename Entity>
	class ObjectTagSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectTagSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				stack = entity;
			}
			else
			{
				assert(false);
			}
		}

		template<typename T>
		void operator()(Entity entity, const T& tag)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("tag", tag));
				archive.finishNode();
			}
		}

	private:
		template<typename Tag>
		void tag0(entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Tag>());
			saver.tag<Tag>(*this);
		}

	public:
		template<typename... Tag>
		void tag(entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Tag>(saver), 0)... };
			(void)accumulator;
		}
	};

	template <typename Entity>
	class ObjectComponentSnapshot
	{
	private:
		cereal::JSONOutputArchive& archive;
		Entity stack = 0;

	public:
		ObjectComponentSnapshot(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	public:
		void operator()(Entity entity)
		{
			if (stack == 0)
			{
				archive.startNode();
				archive.makeArray();
				stack = entity;
			}
			else
			{
				assert(false);
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

		template<typename T>
		void operator()(Entity entity, const T& component)
		{
			if (stack == 0)
			{
				assert(false);
			}
			else
			{
				stack--;
				archive.startNode();
				archive(cereal::make_nvp("id", entity));
				archive(cereal::make_nvp("component", component));
				archive.finishNode();
			}
			if (stack == 0)
			{
				archive.finishNode();
			}
		}

	private:
		template<typename Component>
		void component0(entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Component>());
			saver.component<Component>(*this);
		}

	public:
		template<typename... Component>
		void component(entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Component>(saver), 0)... };
			(void)accumulator;
		}
	};
}