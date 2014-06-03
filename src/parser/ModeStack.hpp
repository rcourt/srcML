/**
 * @file ModeStack.hpp
 *
 * @copyright Copyright (C) 2004-2014 SDML (www.srcML.org)
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

#ifndef MODE_STACK_HPP
#define MODE_STACK_HPP

#include "TokenParser.hpp"
#include "srcMLState.hpp"

/**
 * ModeStack
 *
 * Class representing a stack of modes that direct parsing.
 * Modes also keep srcMLState such as open parethesis/curly braces/tags.
 */
class ModeStack : public TokenParser {

public:

    #include "Mode.hpp"

   /**
     * ModeStack
     * @param ptp the token parser
     *
     * Constructor.  Create mode stack from TokenParser and current language.
     */
    ModeStack(TokenParser* ptp)
        : parser(ptp)
    {}

    /**
     * ~ModeStack
     *
     * Destructor.  Ends all open modes/states.
     */
    ~ModeStack() {

        // end all modes
        endAllModes();

    }

     /** token parser */
    TokenParser* parser;

    /** stack of states/modes */
    std::stack<srcMLState> st;

protected:

    /**
     * currentState
     *
     * Get the current state (const no modification).
     *
     * @returns the current state.
     */
    const srcMLState& currentState() const {
        if (st.empty())
            throw Segmentation_Fault();

        return st.top();
    }

    /**
     * currentState
     *
     * Delegate to get the current state (allow modification).
     *
     * @returns the current state.
     */
    srcMLState& currentState() {
        if (st.empty())
            throw Segmentation_Fault();

        return st.top();
    }

    /**
     * startNewMode
     * @param m new modes to start
     *
     * Delegate to create a new mode and place it on top of stack.
     */
    void startNewMode(const srcMLState::MODE_TYPE& m) {

        // prepare for the new stack
        st.push(srcMLState(m, !empty() ? getTransparentMode() : 0, !empty() ? getMode() : 0));
    }

    /**
     * endMode
     *
     * Delegate to remove the current mode (pop from stack).
     */
    void endMode() {

        if (st.size() == 1)
            throw Segmentation_Fault();

        popMode();
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    /**
     * endMode
     * @param m mode to end
     *
     * Delegate to remove the current mode m (pop from stack).
     * Does not actually check or use m.
     */
    void endMode(const srcMLState::MODE_TYPE& m) {

        if (st.size() <= 1)
            throw Segmentation_Fault();

        popMode();
    }

#pragma GCC diagnostic pop

    /**
     * endLastMode
     *
     * Delegate to ends a mode (not necessarily last) popping from stack.
     */
    void endLastMode() {

        popMode();
    }

    /**
     * getMode
     *
     * Delegate to get the current mode from stack (top of stack).
     *
     * @returns the current mode.
     */
    srcMLState::MODE_TYPE getMode() const {

        return !st.empty() ? st.top().getMode() : 0;
    }

    /**
     * getPrevMode
     *
     * Delegate to get the previousmode from stack (top of stack).
     *
     * @returns the previous mode.
     */
    srcMLState::MODE_TYPE getPrevMode() const {

        return !st.empty() ? st.top().getMode() : 0;
    }

    /**
     * getTransparentMode
     *
     * Delegate to get the transparent mode from stack (top of stack).
     *
     * @returns the transparent mode.
     */
    srcMLState::MODE_TYPE getTransparentMode() const {

        return !st.empty() ? st.top().getTransparentMode() : 0;
    }

    /**
     * setMode
     * @param m modes to add to current mode
     *
     * Add the modes m to the current modes.
     */
    void setMode(const srcMLState::MODE_TYPE& m) {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().setMode(m);
    }

    /**
     * clearMode
     * @param m modes to remove modes from current mode
     *
     * Delegate to removes the modes m from the current modes.
     */
     void clearMode(const srcMLState::MODE_TYPE& m) {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().clearMode(m);
    }

    /**
     * push
     * @param id of open element to add to current mode open element stack
     *
     * Delegate to add the open element id to the top of the open element of the current mode.
     */
    void push(const srcMLState::MODE_TYPE& id) {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().push((int) id);
    }

    /**
     * pop
     *
     * Delegate to remove the top open element form the current mode.
     */
     void pop() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().pop();
    }

    /**
     * size
     *
     * Delegate to get the current modes number of open elements
     *
     * @returns the number of open element in current mode.
     */
    int size() const {

        return (int)st.size();
    }

    /**
     * empty
     *
     * Delegate Predicte method to test if state stack is empty
     *
     * @returns if states in stack.
     */
    bool empty() const {

        return st.empty();
    }

   /**
     * inMode
     * @param m mode to test if currently in
     *
     * Delegate predicte method to test if currently in mode m.
     *
     * @returns if in mode m.
     */
    bool inMode(const srcMLState::MODE_TYPE& m) const {

        return !st.empty() ? st.top().inMode(m) : false;
    }

    /**
     * inPrevMode
     * @param m mode to test if previously in
     *
     * Delegate predicte method to test if previously in mode m.
     *
     * @returns if in previous mode m.
     */
    bool inPrevMode(const srcMLState::MODE_TYPE& m) const {

        return st.size() > 1 ? st.top().inPrevMode(m) : false;
    }

     /**
     * inTransparentMode
     * @param m mode to test if transaprently in
     *
     * Delegate predicte method to test if transparently in mode m.
     *
     * @returns if in transparent mode m.
     */
    bool inTransparentMode(const srcMLState::MODE_TYPE& m) const {

        return !st.empty() ? st.top().inTransparentMode(m) : false;
    }

    /**
     * getParen
     *
     * Delegate to get the open parenthesis in current mode.
     *
     * @returns number of open parethesis in current mode
     */
    int getParen() const {
        return !st.empty() ? st.top().getParen() : 0;
    }

    /**
     * incParen
     *
     * Delegate to increment the number of open parenthesis.
     */
    void incParen() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().incParen();
    }

    /**
     * decParen
     *
     * Delegate to decrement the number of open parenthesis.
     */
    void decParen() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().decParen();
    }

    /**
     * getCurly
     *
     * Delegate to get the open curly braces in current mode.
     *
     * @returns number of open curly braces in current mode
     */
    int getCurly() const {
        return !st.empty() ? st.top().getCurly() : 0;
    }

    /**
     * incCurly
     *
     * Delegate to increment the number of open curly braces.
     */
    void incCurly() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().incCurly();
    }

    /**
     * decCurly
     *
     * Delegate to decrement the number of open curly braces.
     */
    void decCurly() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().decCurly();
    }

    /**
     * getTypeCount count
     *
     * Delegate to get the types in current mode.
     *
     * @returns number of types in current mode
     */
    int getTypeCount() const {
        return !st.empty() ? st.top().getTypeCount() : 0;
    }

    /**
     * setTypeCount count
     * @param n the number of types to set to
     *
     * Delegate to set the types in current mode.
     */
    void setTypeCount(int n) {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().setTypeCount(n);
    }

    /**
     * incTypeCount
     *
     * Delegate to increment the number of types.
     */
    void incTypeCount() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().incTypeCount();
    }

    /**
     * decTypeCount
     *
     * Delegate to decrement the number of types.
     */
    void decTypeCount() {
        if (st.empty())
            throw Segmentation_Fault();

        st.top().decTypeCount();
    }

    /**
     * endAllModes
     *
     * End all modes/states on stack.
     */
    void endAllModes() {

        // end all modes
        while (!st.empty()) {
            endMode(getMode());
        }
    }

    /**
     * popMode
     *
     * Delegate to remove the current mode/state from stack.
     */
    void popMode() {
        if (st.empty())
            throw Segmentation_Fault();

        // close all open elements
        while (!st.empty() && !st.top().openelements.empty()) {
            parser->endElement(st.top().openelements.top());
        }

        st.pop();
    }

    /**
     * endTopMode
     *
     * Delegate to remove/pop the mode on the top of the stack.
     * Equivalent to endMode.
     */
    void endTopMode() {

        endMode();
    }

    /**
     * replaceMode
     * @param oldm modes to clear
     * @param newm modes to set
     *
     * Clear the current modes oldm, and sets then sets the modes new.
     */
    void replaceMode(const srcMLState::MODE_TYPE& oldm, const srcMLState::MODE_TYPE& newm) {

        clearMode(oldm);
        setMode(newm);
    }

    /**
     * dupDownOverMode
     * @param m mode to stop on
     *
     * Duplicate modes on stack down to and including m.
     */
    void dupDownOverMode(const srcMLState::MODE_TYPE& m) {

        std::list<srcMLState> alist;
        while(!(st.top().getMode() & m)) {

            alist.push_front(st.top());
            st.pop();

        }

        alist.push_front(st.top());
        st.pop();


        alist.front().setMode(MODE_TOP | MODE_END_AT_ENDIF);
        for(std::list<srcMLState>::iterator i = alist.begin(); i != alist.end(); ++i) {
            i->setMode(MODE_END_AT_ENDIF);
            st.push(*i);
        }

        alist.front().openelements = std::stack<int>();
        for(std::list<srcMLState>::iterator i = alist.begin(); i != alist.end(); ++i) {
            i->setMode(MODE_ISSUE_EMPTY_AT_POP);
            st.push(*i);

        }


    }

    /**
    * endDownToMode
    * @param mode mode to end down to
    *
    * End elements down to but not including the mode mode.
    */
    void endDownToMode(const srcMLState::MODE_TYPE& mode) {

        if (!inTransparentMode(mode))
            return;

        while (size() > 1 && !inMode(mode))
            endMode();
    }

    /**
     * endDownToModeSet
     * @param mode mode to end down to
     *
     * End elements down to but not including the mode mode.
     */
     void endDownToModeSet(const srcMLState::MODE_TYPE& mode) {

        //  if (getTransparentMode() & (mode == 0))
        //      return;

        while (size() > 1 && (getMode() & mode) == 0)
            endMode();
    }

    /**
     * endDownOverMode
     * @param mode mode to end down to
     *
     * End elements down to and including the mode mode.
     */
     void endDownOverMode(const srcMLState::MODE_TYPE& mode) {

        if (!inTransparentMode(mode))
            return;

        while (size() > 1 && !inMode(mode))
            endMode();
        endMode();
    }

    /**
     * endWhileMode
     * @param mode mode to down over
     *
     * End elements while in mode mode.
     */
     void endWhileMode(const srcMLState::MODE_TYPE& mode) {

        while (size() > 1 && inMode(mode))
            endMode();
    }

};

#endif