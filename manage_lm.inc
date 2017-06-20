<?php

function accounting_lm_list_forms() {
  global $user;
  $query = db_query('SELECT * FROM {lab_migration_form_post}');

  $header_row = array('Date', 'Username', 'Lab Title', 'Status', 'Post Service', 'Tracking Details');

  while ($val = $query->fetchObject()) {
    $user_data = db_query('SELECT * FROM {users} WHERE uid = (:uid)', array(':uid' => $val->user_id));
    $user_data = $user_data->fetchObject();
    $proposal_data = db_query('SELECT * FROM {lab_migration_proposal} WHERE id = (:pid)', array(':pid' => $val->proposal_id));
    $proposal_data = $proposal_data->fetchObject();
    $service_data = db_query('SELECT * FROM {postal_services} WHERE id = (:sid)', array(':sid' => $val->post_service));
    $service_data = $service_data->fetchObject();

    $rows[] = array(
                date('d-m-Y',strtotime($val->timestamp)),
                $user_data->name,
                $proposal_data->lab_title,
                0,
                l($service_data->service_name, $service_data->tracking_url),
                $val->tracking_details,
              );
  }

  return theme('table', array('header' => $header_row, 'rows' => $rows));
}


// function accounting_manage_lm_form($form, $form)