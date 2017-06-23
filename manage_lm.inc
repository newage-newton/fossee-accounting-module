<?php

function accounting_lm_list_forms() {

  $header_row = array('Date', 'Username', 'Lab Title', 'Post Service', 'Tracking Details', 'Status', 'Cheque Details');

  while ($val = $query->fetchObject()) {
    $user_data = db_query('SELECT * FROM {users} WHERE uid = (:uid)', array(':uid' => $val->user_id));
    $user_data = $user_data->fetchObject();
    $proposal_data = db_query('SELECT * FROM {lab_migration_proposal} WHERE id = (:pid)', array(':pid' => $val->proposal_id));
    $proposal_data = $proposal_data->fetchObject();
    $service_data = db_query('SELECT * FROM {postal_services} WHERE id = (:sid)', array(':sid' => $val->post_service));
    $service_data = $service_data->fetchObject();
    $cheque_data = db_query('SELECT * FROM {lab_migration_cheque} WHERE form_id = (:pid)', array(':pid' => $val->id));
    $cheque_data = $cheque_data->fetchObject();

    if($val->status == 0)
      $status = "Sent";
    if($val->status == 1)
      $status = "Received";

    if(is_null($cheque_data->status) || $cheque_data->status == 0) 
      $cheque_details = "Not Dispatched";
    elseif($cheque_data->status == 1)
      $cheque_details = "Dispatched";
    elseif($cheque_data->status == 2)
      $cheque_details = "Received";

    $rows[] = array(
                date('d-m-Y',strtotime($val->timestamp)),
                $user_data->name,
                l($proposal_data->lab_title, "accounting/manage_forms/lm/form/" . check_plain($val->id)),
                l($service_data->service_name, $service_data->tracking_url),
                $val->tracking_details,
                $status,
                $cheque_details,
              );
  }
  return theme('table', array('header' => $header_row, 'rows' => $rows));
}

function accounting_manage_lm_form($form, &$form_state) {
  $id = arg(4);
  $query = db_query("SELECT * FROM {lab_migration_form_post} WHERE id = (:id)", array(':id' => $id)) -> fetchObject();

  if(!is_null($query)) {
    $proposal_data = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = (:pid)", array(':pid' => $query->proposal_id))->fetchObject();
    $user_data = db_query("SELECT * FROM {users} WHERE uid = (:uid)", array(':uid' => $query->user_id))->fetchObject();
    $service_data = db_query("SELECT * FROM {postal_services} WHERE id = (:sid)", array(':sid' => $query->post_service))->fetchObject();
    $contact_data = db_query("SELECT * FROM {lab_migration_contact_details} WHERE user_id = (:uid)", array(':uid' => $query->user_id))->fetchObject();
    $cheque_data = db_query("SELECT * FROM {lab_migration_cheque} WHERE form_id = (:fid)", array(':fid' => $id))->fetchObject();

    if($cheque_data == "")
      $cheque_status = 'Not Dispatched';
    elseif($cheque_data->cheque_status == 0)
      $cheque_status = 'Not Dispatched';
    elseif($cheque_data->cheque_status == 1)
      $cheque_status = 'Dispatched';
    else
      $cheque_status = 'Received';

    $form['lab_tite'] = array(
      '#type' => 'item',
      '#title' => t('Lab Title'),
      '#markup' => $proposal_data->lab_title,
    );

    $form['username'] = array(
      '#type' => 'item',
      '#title' => t('Username'),
      '#markup' => $user_data->name,
    );

    $header_row = array('Form Name', 'Status');
    $rows = array(
      array('Copyright Transfer Form', $query->copyright == 0? 'Not Sent':'Sent'),
      array('Receipt', $query->receipt == 0? 'Not Sent':'Sent'),
      array('Undertaking - Teacher', $query->undertaking_teacher == 0? 'Not Sent':'Sent'),
      array('Undertaking - HOD', $query->undertaking_hod == 0? 'Not Sent':'Sent'),
      array('Undertaking - Principal', $query->undertaking_principal == 0? 'Not Sent':'Sent'),
      array('Lab Migration Application Form', $query->application == 0? 'Not Sent':'Sent'),
    );

    $form['docs_sent'] = array(
      '#type' => 'item',
      '#title' => t('Documents Sent by User'),
      '#markup' => theme('table', array('header' => $header_row, 'rows' => $rows)),
    );
    
    $form ['post_service'] = array(
      '#type' => 'item', 
      '#title' => t('Post Service'), 
      '#markup' => l($service_data->service_name, $service_data->tracking_url)
    );

    $form['tracking_details'] = array(
      '#type' => 'item',
      '#title' => t('Tracking Details'), 
      '#markup' => $query->tracking_details,
    );

    $form['status'] = array(
      '#type' => 'radios',
      '#options' => array(0 => 'Not Received', 1 => 'Received'),
      '#default_value' => $query->status,
      'ajax' => array(
        'callback' => 'status_modify_ajax',
      ),
    );

    $form['contact_details'] = array(
      '#type' => 'item',
      '#title' => t('Contact Details'), 
      '#markup' => t("Phone: @phone <br> Email: @email_id <br> Address: @address", array('@phone' => $contact_data->phone, '@email_id' => $contact_data->email_id, '@address' => $contact_data->address)),
    );

    $form['cheque_status'] = array(
      '#type' => 'item',
      '#title' => t('Cheque Status'),
      '#markup' => $cheque_status,
    );

    $form['cheque'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div id="edit-cheque-details">',
      '#suffix' => '</div>',
    );
      
    $form['cheque']['cheque_number'] = array(
      '#type' => 'textfield', 
      '#title' => t('Enter cheque number'),
    );

    $form['cheque']['cheque_date'] = array(
      '#type' => 'date',
      '#title' => t('Enter date on cheque'),
    );

     $form['cheque']['cheque_amount'] = array(
      '#type' => 'textfield',
      '#title' => t('Enter Cheque amount'),
    );

    $form['cheque']['cheque_bank'] = array(
      '#type' => 'textfield',
      '#title' => t('Enter Bank Name'),
    );

    if($cheque_data != "") {
      $form['cheque']['cheque_number']['#default_value'] = $cheque_data->cheque_number;
      $form['cheque']['cheque_number']['#disabled'] = TRUE;
      $form['cheque']['cheque_date']['#default_value'] = $cheque_data->cheque_date;
      $form['cheque']['cheque_number']['#disabled'] = TRUE;
      $form['cheque']['cheque_amount']['#default_value'] = $cheque_data->cheque_amount;
      $form['cheque']['cheque_amount']['#disabled'] = TRUE;
      $form['cheque']['cheque_bank']['#default_value'] = $cheque_data->cheque_bank;
      $form['cheque']['cheque_bank']['#disabled'] = TRUE;
    }

    $form['edit_cheque_details'] = array(
      '#type' => 'button',
      '#value' => t('Edit Cheque Details'),
      '#name' => 'edit_cheque',
      '#ajax' => array(
        'wrapper' => 'edit-cheque-details',
      ),
      '#submit' => '',
    );

    if(isset($form_state['values']['edit_cheque'])) {
      $form['cheque']['cheque_number']['#disabled'] = FALSE;
      $form['cheque']['cheque_date']['#disabled'] = FALSE;
      $form['cheque']['cheque_amount']['#disabled'] = FALSE;
      $form['cheque']['cheque_bank']['#disabled'] =FALSE;
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Submit Changes',
    );
  }

  return $form;
}

function status_modify_ajax($form, &$form_state) {
  $id = arg(4);
  if($form_state['values']['status'] == 1)
    db_update('lab_migration_form_post')
      ->fields(array(
        'status' == 1,
      ))
      ->condition('id', $id)
      ->execute();
  drupal_goto('accounting/manage_forms/lm/form/' . $id);
}

function accounting_manage_lm_form_validate($form, &$form_state) {
  if(!filter_var($form_state['values']['cheque_number'], FILTER_VALIDATE_INT))
    form_set_error('cheque_number', t('Cheque number should only contain digits 0-9.'));

  if(!filter_var($form_state['values']['cheque_amount'], FILTER_VALIDATE_INT))
    form_set_error('cheque_amount', t('Cheque amount should only contain digits 0-9.'));
}

function accounting_manage_lm_form_submit($form, &$form_state) {
  $id = arg(4);
  db_update('lab_migration_form_post')
    ->fields(array(
      'status' => $form_state['values']['status'],
    ))
    ->condition('id', $id)
    ->execute();

  $query = db_query('SELECT * FROM {lab_migration_cheque} WHERE form_id = (:fid)', array(':fid' => $id));
  if ($query->rowCount() == 0) {
    db_insert('lab_migration_cheque')
      ->fields(array(
        'form_id' => $id,
        'cheque_number' => $form_state['values']['cheque_number'],
        'cheque_amount' => $form_state['values']['cheque_amount'],
        'cheque_date' => date('Y/m/d', strtotime($form_state['values']['cheque_date']['day'] . '-' . $form_state['values']['cheque_date']['month']. '-' . $form_state['values']['cheque_date']['year'])),
        'cheque_bank' => $form_state['values']['cheque_bank'],
        'cheque_status' => 1,
      ))
      ->execute();
  }
  else {
    db_update('lab_migration_cheque')
      ->fields(array(
        'cheque_number' => $form_state['values']['cheque_number'],
        'cheque_amount' => $form_state['values']['cheque_amount'],
        'cheque_date' => date('Y/m/d', strtotime($form_state['values']['cheque_date']['day'] . '-' . $form_state['values']['cheque_date']['month']. '-' . $form_state['values']['cheque_date']['year'])),
        'cheque_bank' => $form_state['values']['cheque_bank'],
        'cheque_status' => 1,
      ))
      ->condition('form_id', $id)
      ->execute();
  }
}