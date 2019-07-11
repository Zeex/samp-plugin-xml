#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"

#include "tinyxpath/tinyxml.h"
#include "tinyxpath/xpath_static.h"

#include <malloc.h> 
#include <stdlib.h>

typedef void (*logprintf_t)(const char *format, ...);
logprintf_t logprintf;

extern void *pAMXFunctions;

const char *CompletePath(const char path[], const char prefix[]) {
	static char full_path[FILENAME_MAX];

	while (*path == '.' || *path == '\\' || *path == '/')
		path++;

	strncpy(full_path, prefix, FILENAME_MAX);
	strncat(full_path, path, FILENAME_MAX);
	return full_path;
}

namespace natives {

	using namespace TinyXPath;

	// native XMLNode:XML_CreateDocument(const filename[], const version[] = "1.0", const encoding[] = "ISO-8859-1");
	cell AMX_NATIVE_CALL XML_CreateDocument(AMX *amx, cell params[]) {
		char *filename, *version, *encoding;
		amx_StrParam(amx, params[1], filename);
		amx_StrParam(amx, params[2], version);
		amx_StrParam(amx, params[3], encoding);

		TiXmlDocument *doc = new TiXmlDocument(CompletePath(filename, "scriptfiles/"));
		TiXmlDeclaration *decl = new TiXmlDeclaration(version, encoding, "");
		doc->LinkEndChild(decl);
		return (cell)doc;
	}

	// native XMLNode:XML_LoadDocument(const filename[]);
	cell AMX_NATIVE_CALL XML_LoadDocument(AMX *amx, cell params[]) {
		char *filename;
		amx_StrParam(amx, params[1], filename);

		TiXmlDocument *doc = new TiXmlDocument;
		doc->LoadFile(CompletePath(filename, "scriptfiles/"));

		if (doc->Error()) {
			logprintf("XML Error %d: %s (line: %d, col: %d)", 
					doc->ErrorId(), doc->ErrorDesc(), doc->ErrorRow(), doc->ErrorCol());
		}
		return (cell)doc; 
	}

	// native bool:XML_SaveDocument(XMLNode:doc, const filename[] = "");
	cell AMX_NATIVE_CALL XML_SaveDocument(AMX *amx, cell params[]) {
		char *filename;
		amx_StrParam(amx, params[2], filename);

		if (filename == NULL) {
			return (cell)((TiXmlDocument*)params[1])->SaveFile();
		}
		else {
			return (cell)((TiXmlDocument*)params[1])->SaveFile(filename);
		}
	}

	// native XML_UnloadDocument(XMLNode:doc);
	cell AMX_NATIVE_CALL XML_UnloadDocument(AMX *amx, cell params[]) {
		delete (TiXmlDocument*)params[1];
		return 0;
	}

	// native XMLNode:XML_CreateNode(XMLNode:parent, const value[], e_XML_NODE_TYPE:type = e_XML_NODE_ELEMENT);
	cell AMX_NATIVE_CALL XML_CreateNode(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *child, *parent = (TiXmlNode*)params[1];
		switch (params[3]) {
			case TiXmlNode::ELEMENT:
				child = new TiXmlElement(value);
				break;
			case TiXmlNode::COMMENT:
				child = new TiXmlComment(value);
				break;
			case TiXmlNode::TEXT:
				child = new TiXmlText(value);
				break;
			default:
				child = new TiXmlElement(value);
				break;
		}
		parent->LinkEndChild(child);
		return (cell)child;
	}

	// native bool:XML_RemoveChild(XMLNode:parent, XMLNode:child);
	cell AMX_NATIVE_CALL XML_RemoveChild(AMX *amx, cell params[]) {
		TiXmlNode *parent = (TiXmlNode*)params[1];
		TiXmlNode *child  = (TiXmlNode*)params[2];
		return (cell)parent->RemoveChild(child);
	}

	// native XML_DestroyNode(XMLNode:node);
	cell AMX_NATIVE_CALL XML_DestroyNode(AMX *amx, cell params[]) {
		TiXmlNode *node = (TiXmlNode*)params[1];
		delete node;
		return 0;
	}

	// native XMLNode:XML_GetFirstChild(XMLNode:node, const value[] = "");
	cell AMX_NATIVE_CALL XML_GetFirstChild(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *node = (TiXmlNode*)params[1];
		if (value != NULL) {
			// Find by value
			return (cell)node->FirstChild(value);
		}
		else {
			return (cell)node->FirstChild();
		}
	}

	// native XMLNode:XML_GetLastChild(XMLNode:node, const value[] = "");	
	cell AMX_NATIVE_CALL XML_GetLastChild(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *node = (TiXmlNode*)params[1];
		if (value != NULL) {
			// Find by value
			return (cell)node->LastChild(value);
		}
		else {
			return (cell)node->LastChild();
		}		
	}

	// native XMLNode:XML_GetNextSibling(XMLNode:node, const value[] = "");
	cell AMX_NATIVE_CALL XML_GetNextSibling(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *node = (TiXmlNode*)params[1];
		if (value != 0) {
			return (cell)node->NextSibling(value);
		}
		else {
			return (cell)node->NextSibling();
		}
	}

	// native XMLNode:XML_GetPreviousSibling(XMLNode:node, const value[] = "");
	cell AMX_NATIVE_CALL XML_GetPreviousSibling(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *node = (TiXmlNode*)params[1];
		if (value != 0) {
			return (cell)node->PreviousSibling(value);
		}
		else {
			return (cell)node->PreviousSibling();
		}
	}

	// native XMLNode:XML_GetParent(XMLNode:node);
	cell AMX_NATIVE_CALL XML_GetParent(AMX *amx, cell params[]) {
		return (cell)(((TiXmlNode*)params[1])->Parent());
	}

	// native XML_GetValue(XMLNode:node, value[], maxlen = sizeof(value));
	cell AMX_NATIVE_CALL XML_GetValue(AMX *amx, cell params[]) {
		cell *addr;
		amx_GetAddr(amx, params[2], &addr);
		amx_SetString(addr, ((TiXmlNode*)params[1])->Value(), 0, 0, (int)params[3]);
		return 0;
	}

	// native XML_SetValue(XMLNode:node, const value[]);
	cell AMX_NATIVE_CALL XML_SetValue(AMX *amx, cell params[]) {
		char *value;
		amx_StrParam(amx, params[2], value);

		TiXmlNode *node = (TiXmlNode*)params[1];
		if (node->Type() == TiXmlNode::DOCUMENT) {
			node->SetValue(CompletePath(value, "scriptfiles/"));
		}
		else {
			node->SetValue(value);
		}
		return 0;
	}

	// native XML_GetAttribute(XMLNode:element, const name[], value[], maxlen = sizeof(value));
	cell AMX_NATIVE_CALL XML_GetAttribute(AMX *amx, cell params[]) {
		char *name;
		amx_StrParam(amx, params[2], name);

		cell *addr;
		amx_GetAddr(amx, params[3], &addr);
		amx_SetString(addr, ((TiXmlNode*)params[1])->ToElement()->Attribute(name), 0, 0, params[4]);	
		return 0;
	}

	// native XML_SetAttribute(XMLNode:element, const name[], const value[]);
	cell AMX_NATIVE_CALL XML_SetAttribute(AMX *amx, cell params[]) {
		char *name, *value;
		amx_StrParam(amx, params[2], name);
		amx_StrParam(amx, params[3], value);

		((TiXmlNode*)params[1])->ToElement()->SetAttribute(name, value);
		return 0;
	}

	// native XML_RemoveAttribute(XMLNode:element, const name[]);
	cell AMX_NATIVE_CALL XML_RemoveAttribute(AMX *amx, cell params[]) {
		char *name;
		amx_StrParam(amx, params[2], name);

		((TiXmlNode*)params[1])->ToElement()->RemoveAttribute(name);	
		return 0;
	}

	// native e_XML_NODE_TYPE:XML_GetNodeType(XMLNode:node);
	cell AMX_NATIVE_CALL XML_GetNodeType(AMX *amx, cell params[]) {
		return (cell)((TiXmlNode*)params[1])->Type();
	}

	// native XML_GetXPathInt(XMLNode:root, const xpath[]);
	cell AMX_NATIVE_CALL XML_GetXPathInt(AMX *amx, cell params[]) {
		char *xpath;
		amx_StrParam(amx, params[2], xpath);
		return (cell)i_xpath_int((TiXmlNode*)params[1], xpath);
	}

	// native bool:XML_GetXPathBool(XMLNode:root, const xpath[]);
	cell AMX_NATIVE_CALL XML_GetXPathBool(AMX *amx, cell params[]) {
		char *xpath;
		amx_StrParam(amx, params[2], xpath);
		return (cell)o_xpath_bool((TiXmlNode*)params[1], xpath);
	}

	// native Float:XML_GetXPathFloat(XMLNode:root, const xpath[]);
	cell AMX_NATIVE_CALL XML_GetXPathFloat(AMX *amx, cell params[]) {
		char *xpath;
		amx_StrParam(amx, params[2], xpath);
        float result = d_xpath_double((TiXmlNode*)params[1], xpath);
		return amx_ftoc(result);
	}

	// native XML_GetXPathString(XMLNode:root, const xpath[], result[], maxlen = sizeof(result));
	cell AMX_NATIVE_CALL XML_GetXPathString(AMX *amx, cell params[]) {
		char *xpath;
		amx_StrParam(amx, params[2], xpath);

		TiXmlNode *node = (TiXmlNode*)params[1];
		const char *result = S_xpath_string(node, xpath).c_str();

		cell *addr;
		amx_GetAddr(amx, params[3], &addr);
		amx_SetString(addr, result, 0, 0, (int)params[4]);
		return 0;
	}

	// native XMLNode:XML_GetXPathNode(XMLNode:root, const xpath[]);
	cell AMX_NATIVE_CALL XML_GetXPathNode(AMX *amx, cell params[]) {
		char *xpath;
		amx_StrParam(amx, params[2], xpath);
		return (cell)XNp_xpath_node((TiXmlNode*)params[1], xpath);
	}

	static const AMX_NATIVE_INFO all[] = {
		// tree
		{"XML_CreateDocument",     XML_CreateDocument},
		{"XML_LoadDocument",       XML_LoadDocument},
		{"XML_SaveDocument",       XML_SaveDocument},
		{"XML_UnloadDocument",     XML_UnloadDocument},
		{"XML_CreateNode",         XML_CreateNode},
		{"XML_RemoveChild",        XML_RemoveChild},
		{"XML_DestroyNode",        XML_DestroyNode},
		{"XML_GetFirstChild",      XML_GetFirstChild},
		{"XML_GetLastChild",       XML_GetLastChild},
		{"XML_GetNextSibling",     XML_GetNextSibling},
		{"XML_GetPreviousSibling", XML_GetPreviousSibling},
		{"XML_GetParent",          XML_GetParent},
		{"XML_GetValue",           XML_GetValue},
		{"XML_SetValue",           XML_SetValue},
		{"XML_GetAttribute",       XML_GetAttribute},
		{"XML_SetAttribute",       XML_SetAttribute},
		{"XML_RemoveAttribute",    XML_RemoveAttribute},
		{"XML_GetNodeType",        XML_GetNodeType},
		// xpath
		{"XML_GetXPathInt",        XML_GetXPathInt},
		{"XML_GetXPathBool",       XML_GetXPathBool},
		{"XML_GetXPathFloat",      XML_GetXPathFloat},
		{"XML_GetXPathString",     XML_GetXPathString},
		{"XML_GetXPathNode",       XML_GetXPathNode},
		{NULL,                     NULL}
	};

} // namespace natives

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	logprintf("  XML plugin 1.0");
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	return amx_Register(amx, natives::all, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	return AMX_ERR_NONE;
}



