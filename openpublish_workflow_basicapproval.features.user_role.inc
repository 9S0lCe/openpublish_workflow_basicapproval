<?php
/**
 * @file
 * openpublish_roles.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function openpublish_workflow_basicapproval_user_default_roles() {
  $roles = array();

  // Exported role: editor.
  $roles['editor'] = array(
    'name' => 'editor',
    'weight' => '3',
  );

  // Exported role: staff.
  $roles['staff'] = array(
    'name' => 'staff',
    'weight' => '4',
  );

  return $roles;
}

