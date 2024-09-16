#include "../../../ns-3-dev/contrib/NGAP/src/ext/yaml-cpp/node/node.h" 
#include <ns3/nodebuilder.h>
#include <ns3/nodeevents.h>

namespace YAML {
Node Clone(const Node& node) {
  NodeEvents events(node);
  NodeBuilder builder;
  events.Emit(builder);
  return builder.Root();
}
}  // namespace YAML
