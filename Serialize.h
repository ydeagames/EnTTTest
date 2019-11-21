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
		template<typename Tag>
		void tag0(entt::SnapshotLoader<Entity>& loader) {
			const char* name = IdentifierResolver::name<Tag>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				loader.tag<Tag>(*this);
			}
		}

	public:
		template<typename... Tags>
		void tag(entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Tags>(loader), 0)... };
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
		template<typename... Components>
		void component(entt::SnapshotLoader<Entity>& loader) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(loader), 0)... };
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
		void tag0(const entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Tag>());
			saver.tag<Tag>(*this);
		}

	public:
		template<typename... Tags>
		void tag(const entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (tag0<Tags>(saver), 0)... };
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
		void component0(const entt::Snapshot<Entity>& saver) {
			archive.setNextName(IdentifierResolver::name<Component>());
			saver.component<Component>(*this);
		}

	public:
		template<typename... Components>
		void component(const entt::Snapshot<Entity>& saver) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Components>(saver), 0)... };
			(void)accumulator;
		}
	};

	template<typename Registry>
	class ComponentClone
	{
	public:
		template<typename Component>
		static void Clone(Registry& reg, typename Registry::entity_type src, typename Registry::entity_type dst)
		{
			if (reg.has<Component>(src))
			{
				std::stringstream buffer;
				{
					cereal::BinaryOutputArchive output(buffer);
					auto& srcComponent = reg.get<Component>(src);
					output(srcComponent);
				}
				{
					cereal::BinaryInputArchive input(buffer);
					auto& dstComponent = reg.accommodate<Component>(dst);
					input(dstComponent);
				}
			}
		}
	};

	template <typename Registry>
	class EntityExporter
	{
	private:
		cereal::JSONOutputArchive& archive;

	public:
		EntityExporter(cereal::JSONOutputArchive& archive)
			: archive(archive)
		{
		}

	private:
		template<typename Component>
		void component0(const Registry& reg, typename Registry::entity_type entity) {
			archive.setNextName(IdentifierResolver::name<Component>());
			if (reg.has<Component>(entity))
			{
				auto& component = reg.get<Component>(entity);
				archive(component);
			}
		}

	public:
		template<typename... Component>
		void component(const Registry& reg, typename Registry::entity_type entity) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Component>(reg, entity), 0)... };
			(void)accumulator;
		}
	};

	template <typename Registry>
	class EntityImporter
	{
	private:
		cereal::JSONInputArchive& archive;

	public:
		EntityImporter(cereal::JSONInputArchive& archive)
			: archive(archive)
		{
		}

	private:
		template<typename Component>
		void component0(Registry& reg, typename Registry::entity_type entity) {
			const char* name = IdentifierResolver::name<Component>();
			if (archive.hasName(name))
			{
				archive.setNextName(name);
				auto& component = reg.accommodate<Component>(entity);
				archive(component);
			}
		}

	public:
		template<typename... Component>
		void component(Registry& reg, typename Registry::entity_type entity) {
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (component0<Component>(reg, entity), 0)... };
			(void)accumulator;
		}
	};
}