/**
 * @file UnitDOM.cpp
 *
 * @copyright Copyright (C) 2011-2014 SDML (www.srcML.org)
 *
 * This file is part of the srcML Toolkit.
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_UNITDOM_HPP
#define INCLUDED_UNITDOM_HPP

#include <libxml/SAX2.h>

#include <srcMLHandler.hpp>

#include <string>
#include <vector>

class UnitDOM : public srcMLHandler {
public :

    UnitDOM(OPTION_TYPE options) : rootsize(0), found(false), options(options), error(false) {}

    virtual ~UnitDOM() {}

    virtual OPTION_TYPE getOptions() const { return options; }

    /*
      Called exactly once at beginnning of document  Override for intended behavior.
    */
    virtual void startOutput() = 0;

    /*
      Called exactly once for each unit.  For an archive, not called on the root unit

      Formed unit combines namespaces from root and individual unit.  Full DOM of
      individual unit is provided.  Cleanup of DOM unit is automatic.
    */
    virtual bool apply() = 0;

    /*
      Called exactly once at end of document.  Override for intended behavior.
    */
    virtual void endOutput() = 0;

    // start creating the document and setup output for the units
    virtual void startDocument() {

        ctxt = get_control_handler().getCtxt();

        // apparently endDocument() can be called without startDocument() for an
        // empty element
        found = true;

        // setup output
        startOutput();

        xmlSAX2StartDocument(ctxt);

    }

    // collect namespaces from root unit.  Start to build the tree if OPTION_APPLY_ROOT
    virtual void startRoot(const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
			   int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
			   const xmlChar** attributes, std::vector<srcMLElement> * meta_tags) {

        SAX2srcMLHandler* handler = (SAX2srcMLHandler*)get_control_handler().getCtxt()->_private;
        root = &handler->root;
	this->meta_tags = meta_tags;

        // record namespaces in an extensible list so we can add the per unit
        for (int i = 0; i < nb_namespaces; ++i) {

            data.push_back(namespaces[i * 2]);
            data.push_back(namespaces[i * 2 + 1]);

        }
        rootsize = data.size();

        // if we are building the entire tree, start now
        if (isoption(options, OPTION_APPLY_ROOT)) {

            xmlSAX2StartElementNs(ctxt, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes,
                                  nb_defaulted, attributes);

        }

    }

    // start to create an individual unit, merging namespace details from the root (if it exists)
    virtual void startUnit(const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                           int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                           const xmlChar** attributes) {

        // remove per-unit namespaces
        data.resize(rootsize);

        // combine namespaces from root and local to this unit
        for (int i = 0; i < nb_namespaces; ++i) {

            // make sure not already in
            bool found = false;
            for (std::vector<const xmlChar*>::size_type j = 0; j < data.size() / 2; ++j)
                if (xmlStrEqual(data[j * 2], namespaces[i * 2]) &&
                    xmlStrEqual(data[j * 2 + 1], namespaces[i * 2 + 1])) {
                    found = true;
                    break;
                }

            if (found)
                continue;

            data.push_back(namespaces[i * 2]);
            data.push_back(namespaces[i * 2 + 1]);
        }

	/*

	  This should not be needed since start root should always be called.
        // if applying to entire archive, then just build this node
        if (isoption(options, OPTION_APPLY_ROOT)) {

            // if apply root and not archive then startRootUnit may not have been called
            static bool started = false;
            if(!is_archive && !started) xmlSAX2StartDocument(ctxt);
            started = true;
            xmlSAX2StartElementNs(ctxt, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes,
                                  nb_defaulted, attributes);

            return;
        }
	*/

        // start the document for this unit
        //xmlSAX2StartDocument(ctxt);

        // start the unit (element) at the root using the merged namespaces
        xmlSAX2StartElementNs(ctxt, localname, prefix, URI, (int)(data.size() / 2),
                              &data[0], nb_attributes, nb_defaulted, attributes);

    }

    // build start element nodes in unit tree
    virtual void startElementNs(const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                                int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                                const xmlChar** attributes) {

        xmlSAX2StartElementNs(ctxt, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
    }

    // build end element nodes in unit tree
    virtual void endElementNs(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

        xmlSAX2EndElementNs(ctxt, localname, prefix, URI);
    }

    // characters in unit tree
    virtual void charactersUnit(const xmlChar* ch, int len) {

        xmlSAX2Characters(ctxt, ch, len);
    }

    // characters in unit tree
    virtual void charactersRoot(const xmlChar* ch, int len) {

      if(isoption(options, OPTION_APPLY_ROOT))
        xmlSAX2Characters(ctxt, ch, len);
    }

    // CDATA block in unit tree
    virtual void cdatablock(const xmlChar* ch, int len) {

        xmlSAX2CDataBlock(ctxt, ch, len);
    }

    // comments in unit tree
    virtual void comments(const xmlChar* ch) {

        xmlSAX2Comment(ctxt, ch);
    }

    // end the construction of the unit tree, apply processing, and delete
    virtual void endUnit(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

        // finish building the unit tree
        xmlSAX2EndElementNs(ctxt, localname, prefix, URI);

        // End the document and free it if applied to unit individually
        if(!isoption(options, OPTION_APPLY_ROOT)) {
            xmlSAX2EndDocument(ctxt);

            // apply the necessary processing
            if ((error = !apply()))
                stop_parser();

            // free up the document that has this particular unit
            xmlNodePtr aroot = ctxt->myDoc->children;
            xmlUnlinkNode(ctxt->myDoc->children);
            xmlFreeNodeList(aroot);
            ctxt->myDoc->children = 0;

        }

    }

    // end the construction of the unit tree, apply processing, and delete
    virtual void endRoot(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

        if(isoption(options, OPTION_APPLY_ROOT)) {

  	    // finish building the unit tree
	    xmlSAX2EndElementNs(ctxt, localname, prefix, URI);

	    // End the document and free it if applied to unit individually
            xmlSAX2EndDocument(ctxt);

            // apply the necessary processing
            if ((error = !apply()))
                stop_parser();

            // free up the document that has this particular unit
            xmlNodePtr aroot = ctxt->myDoc->children;
            xmlUnlinkNode(ctxt->myDoc->children);
            xmlFreeNodeList(aroot);
            ctxt->myDoc->children = 0;

        }

    }

    virtual void endDocument() {

        // endDocument can be called, even if startDocument was not for empty input
        if (!found || error)
            return;
	/*
        // end the entire input document and run apply if applied to root.
        if (isoption(options, OPTION_APPLY_ROOT)) {
            xmlSAX2EndDocument(ctxt);

            if ((error = !apply()))
	      stop_parser();

            xmlNodePtr onode = xmlDocGetRootElement(ctxt->myDoc);
            onode->name = NULL;

	    }*/

        // free up the document that has this particular unit
        xmlFreeDoc(ctxt->myDoc);
        ctxt->myDoc = 0;

        // end the output
        endOutput();

}

protected:

    std::vector<const xmlChar*> data;
    std::vector<const xmlChar*>::size_type rootsize;
    bool found;
    OPTION_TYPE options;
    bool error;
    xmlParserCtxtPtr ctxt;
    srcMLElement * root;
    std::vector<srcMLElement> * meta_tags;

};

#endif
