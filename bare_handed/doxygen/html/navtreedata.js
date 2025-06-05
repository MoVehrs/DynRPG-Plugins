/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "BareHanded", "index.html", [
    [ "BareHanded Plugin for RPG Maker 2003 (DynRPG)", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html", [
      [ "Features", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md1", null ],
      [ "Installation", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md2", null ],
      [ "Technical Details", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md3", [
        [ "Value Ranges and Limitations", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md4", null ],
        [ "MaxActorId Setting", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md5", null ],
        [ "Error Handling", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md6", null ],
        [ "Debug Output Format", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md7", null ]
      ] ],
      [ "Configuration", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md8", [
        [ "Debug Options", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md9", null ],
        [ "Actor Weapon Mappings", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md10", [
          [ "Direct Weapon Mapping", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md11", null ],
          [ "Variable-Based Mapping", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md12", null ]
        ] ],
        [ "Error Handling Examples", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md13", null ],
        [ "Debug Output Examples", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md14", null ]
      ] ],
      [ "Event Comment Commands", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md15", [
        [ "1. Unequip Bare Hand Weapon", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md16", null ],
        [ "2. Update Bare Hand Equipment", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md17", null ],
        [ "Important: Using the Change Equipment Event Command", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md18", null ]
      ] ],
      [ "Combat System and Formulas", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md19", [
        [ "Default vs Plugin Combat", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md20", null ]
      ] ],
      [ "Troubleshooting", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md21", null ],
      [ "Credits", "md__c_1_2_users_2_buddy_2_desktop_2_dyn_r_p_g_01_test_2_dyn_plugins_2bare__handed_2_r_e_a_d_m_e.html#autotoc_md22", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"bare__handed_8cpp.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';