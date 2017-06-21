<?php
/**
 * @file
 * Track status of forms sent by post.
 */
function manage_forms_tbc_user_form($form, &$form_state) {
  global $user;
  $user_id = $user->uid;
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('uid', $user_id);
  $query->orderBy('id', 'DESC');
  $result = $query->execute();
  if(!$proposal_data = $result->fetchObject()) {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  }
  $proposal_id = $proposal_data->id;
  $query = db_select('textbook_companion_paper_user');
  $query->fields('textbook_companion_paper_user');
  $query->condition('proposal_id', $proposal_id);
  $result = $query->execute();
  if($form_data = $result->fetchObject()) {
    $form['form_details'] = array(
        '#type' => 'fieldset',
        '#size' => '40',
        '#title' => t('Application Status'),
        //'#collapsible' => FALSE,
        //'#collapsed' => FALSE,
    );
    $form_html .= '<ul>';
    if ($form_data->internship_form)
        $form_html .= '<li><strong>Internship Application: </strong> Form Submitted</li>';
    else
        $form_html .= '<li><strong>Internship Application: </strong> Form Not Submitted </li>';
    if ($form_data->copyright_form)
        $form_html .= '<li><strong>Copyright Application: </strong> Form Submitted</li>';
    else
        $form_html .= '<li><strong>Copyright Application: </strong> Form Not Submitted </li>';
    if ($form_data->undertaking_form)
        $form_html .= '<li><strong>Undertaking Application: </strong> Form Submitted</li>';
    else
        $form_html .= '<li><strong>Undertaking Application: </strong> Form Not Submitted </li>';
    $form_html .= '</ul>';
    $query = db_select('postal_services');
    $query->fields('postal_services');
    $query->condition('id', $form_data->post_service);
    $result = $query->execute();
    $data = $result->fetchObject();
    $form_html .= '<strong>Postal Service : </strong>' . $data->service_name  . '<br>';
    $form_html .= '<strong>Tracking Details: </strong>' . $form_data->tracking_details . '<br>';
    $form_html .= 'Update your contact details ' . l('Here', 'accounting/contact_details/user') . '<br />';
    $form['form_details']['form'] = array(
        '#type' => 'item',
        '#markup' => $form_html,
        //'#title' => t('Application Form Status')
    );
    drupal_set_message("You have sent us your form. We will get back to you after receiving it.", 'status');
  }
  else {
    $form['proposal_id'] = array(
      '#type' => 'hidden',
      '#value' => $proposal_id
    );
    $form['internshipform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Internship Application'),
      '#description' => t('I have sent Internship Application Form.'),
    );
    $form['copyrighttransferform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Copyright Transfer Form'),
      '#description' => t('I have sent Copyright Transfer Form.'),
    );
    $form['undertakingform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Undertaking Form'),
      '#description' => t('I have sent Undertaking Form.'),
    );
    $form['recieptform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Recieved Reciept Form'),
      '#description' => t('I have sent Reciept Form.'),
    );
    $service_options = array();
    $query = db_select('postal_services');
    $query->fields('postal_services');
    $result = $query->execute();
    while($data = $result->fetchObject()){
      $service_options[$data->id] = $data->service_name;
    }
    $form['post_service'] = array(
      '#type' => 'radios',
      '#title' => t('Select your postal/courier service.'),
      '#options' => $service_options,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'tracking_details_ajax',
        'wrapper' => 'tracking-details-div',
        'method' => 'html',
        'effect' => 'fade',
      ),
    );
    $form['tracking_details'] = array(
      '#prefix' => '<div id="tracking-details-div">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );
    if(isset($form_state['values']['post_service'])) {
      $form['tracking_details'] = array(
        '#type' => 'textfield',
        '#title' => 'Enter tracking details',
        '#size' => 30,
        '#prefix' => '<div id="tracking-details-div">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
        '#required' => TRUE,
      );
    }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Send Email')
    );
    $form['cancel'] = array(
      '#type' => 'markup',
      '#value' => l(t('Cancel'), 'manage_proposal/all')
    );
  }
  return $form;
}

function manage_forms_tbc_user_form_submit($form, &$form_state) {
    /*$query ="UPDATE {textbook_companion_paper} SET internship_form = ".$form_state['values']['internshipform'].", copyright_form = ".$form_state['values']['copyrighttransferform'].", undertaking_form= ".$form_state['values']['undertakingform'].", reciept_form= ".$form_state['values']['recieptform']." WHERE proposal_id = ".$form_state['values']['proposal_id'];
    db_query($query);*/
    $query = "INSERT INTO {textbook_companion_paper_user} (internship_form, copyright_form, undertaking_form, reciept_form, proposal_id, post_service, tracking_details) VALUES (:internship_form, :copyright_form, :undertaking_form, :reciept_form, :proposal_id, :post_service, :tracking_details)";
    $args = array(
      ':internship_form' => $form_state['values']['internshipform'],
      ':copyright_form' => $form_state['values']['copyrighttransferform'],
      ':undertaking_form' => $form_state['values']['undertakingform'],
      ':reciept_form' => $form_state['values']['recieptform'],
      ":proposal_id" => $form_state['values']['proposal_id'],
      ':post_service' => $form_state['values']['post_service'],
      ':tracking_details' => $form_state['values']['tracking_details'],
    );
    $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
    ));
    drupal_set_message(t('Proposal Updated'), 'status');
}

function manage_forms_tbc() {
    $form['#redirect'] = FALSE;
    $form['search_cheque'] = array(
        '#type' => 'textfield',
        '#title' => t('Search'),
        '#size' => 48
    );
    $form['submit_cheque'] = array(
        '#type' => 'submit',
        '#value' => t('Search')
    );
    $form['cancel_cheque'] = array(
        '#type' => 'markup',
        '#value' => l(t('Cancel'), '')
    );
    $count = 20;
    /* get pending proposals to be approved */
    $proposal_rows = array();
    /*$proposal_q = "SELECT * FROM {textbook_companion_proposal} ORDER BY id DESC";*/
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->orderBy('id', 'DESC');
    /*$pagerquery = pager_query($proposal_q, $count); */
    $pagerquery = $query->extend('PagerDefault')->limit($count)->execute();
    while ($proposal_data = $pagerquery->fetchObject())
    {
        /* get preference */
        /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
        $preference_data = db_fetch_object($preference_q);*/
        $query = db_select('textbook_companion_preference');
        $query->fields('textbook_companion_preference');
        $query->condition('proposal_id', $proposal_data->id);
        $query->condition('approval_status', 1);
        $query->range(0, 1);
        $result = $query->execute();
        $preference_data = $result->fetchObject();
        /*$preference_q1 = db_query("SELECT * FROM {textbook_companion_proposal} WHERE uid = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
        $preference_data1 = db_fetch_object($preference_q1);*/
        $query = db_select('textbook_companion_proposal');
        $query->fields('textbook_companion_proposal');
        $query->condition('uid', $proposal_data->id);
        $query->condition('proposal_status', 1);
        $query->range(0, 1);
        $result = $query->execute();
        $preference_data1 = $result->fetchObject();
        $proposal_status = '';
        switch ($proposal_data->proposal_status)
        {
            case 0:
                $proposal_status = 'Pending';
                break;
            case 1:
                $proposal_status = 'Approved';
                break;
            case 2:
                $proposal_status = 'Dis-approved';
                break;
            case 3:
                $proposal_status = 'Completed';
                break;
            default:
                $proposal_status = 'Unknown';
                break;
        } //$proposal_data->proposal_status
        $proposal_rows[] = array(
            date('d-m-Y', $proposal_data->creation_date),
            l($proposal_data->full_name, 'user/' . $proposal_data->uid),
            date('d-m-Y', $proposal_data->completion_date),
            l('Form Submission', 'accounting/manage_forms/tbc/paper_submission/' . $proposal_data->id) . ' | ' . l('Cheque Details', 'accounting/cheque_contact/status/' . $proposal_data->id)
        );
    } //$proposal_data = $pagerquery->fetchObject()
    /* check if there are any pending proposals */
    if (!$proposal_rows)
    {
        drupal_set_message(t('There are no proposals.'), 'status');
        return '';
    } //!$proposal_rows
    $proposal_header = array(
        'Date of Submission',
        'Contributor Name',
        'Expected Date of Completion',
        'Status'
    );
    $output = theme('table', array(
        'header' => $proposal_header,
        'rows' => $proposal_rows
    ));
    return $output;
    //doubt
    //return $form;
}

function paper_submission_form($form_state, $proposal_id) {
    global $user;
    $proposal_id = arg(4);
    /* get current proposal */
    /*$preference4_q = db_query("SELECT * FROM {textbook_companion_paper} WHERE proposal_id=".$proposal_id);*/
    $query = db_select('textbook_companion_paper');
    $query->fields('textbook_companion_paper');
    $query->condition('proposal_id', $proposal_id);
    $preference4_q = $query->execute();
    $form1 = 0;
    $form2 = 0;
    $form3 = 0;
    $form4 = 0;
    if ($data = $preference4_q->fetchObject())
    {
        $form1 = $data->internship_form;
        $form2 = $data->copyright_form;
        $form3 = $data->undertaking_form;
        $form4 = $data->reciept_form;
    } //$data = $preference4_q->fetchObject()
    else
    {
        $query = "INSERT INTO {textbook_companion_paper} (proposal_id) values (:proposal_id)";
        $args = array(
            ":proposal_id" => $proposal_id
        );
        $result = db_query($query, $args, array(
            'return' => Database::RETURN_INSERT_ID
        ));
    }
    $form['proposal_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $proposal_id
    );
    $form['internshipform'] = array(
        '#type' => 'checkbox',
        '#title' => t('Recieved Internship Application'),
        '#description' => t('Check if the Internship Application has been recieved.'),
        '#default_value' => $form1
    );
    $form['copyrighttransferform'] = array(
        '#type' => 'checkbox',
        '#title' => t('Recieved Copyright Transfer Form'),
        '#description' => t('Check if the Copyright Transfer Form has been recieved.'),
        '#default_value' => $form2
    );
    $form['undertakingform'] = array(
        '#type' => 'checkbox',
        '#title' => t('Recieved Undertaking Form'),
        '#description' => t('Check if the Undertaking Form has been recieved.'),
        '#default_value' => $form3
    );
    $form['recieptform'] = array(
        '#type' => 'checkbox',
        '#title' => t('Recieved Reciept Form'),
        '#description' => t('Check if the Reciept Form has been recieved.'),
        '#default_value' => $form4
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Send Email')
    );
    return $form;
}

function paper_submission_form_submit($form, &$form_state) {
    /*$query ="UPDATE {textbook_companion_paper} SET internship_form = ".$form_state['values']['internshipform'].", copyright_form = ".$form_state['values']['copyrighttransferform'].", undertaking_form= ".$form_state['values']['undertakingform'].", reciept_form= ".$form_state['values']['recieptform']." WHERE proposal_id = ".$form_state['values']['proposal_id'];
    db_query($query);*/
    $query = db_update('textbook_companion_paper');
    $query->fields(array(
        'internship_form' => $form_state['values']['internshipform'],
        'copyright_form' => $form_state['values']['copyrighttransferform'],
        'undertaking_form' => $form_state['values']['undertakingform'],
        'reciept_form' => $form_state['values']['recieptform']
    ));
    $query->condition('proposal_id', $form_state['values']['proposal_id']);
    $num_updated = $query->execute();
    /************************************************
    Check For the Internship Form is checked or not
    ************************************************/
    if ($form_state['values']['internshipform'] == 1)
    {
        /* sending email */
        $book_user = user_load($proposal_data->uid);
        $param['proposal_completed']['proposal_id'] = $proposal_id;
        $param['proposal_completed']['user_id'] = $proposal_data->uid;
        $email_to = $book_user->mail;
        if (!drupal_mail('textbook_companion', 'internship_form', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Internship Form for Book proposal has been recieved. User has been notified.'), 'status');
    } //$form_state['values']['internshipform'] == 1
    else
    {
        if (!drupal_mail('textbook_companion', 'internship_form_not', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Internship Form for Book proposal has not been recieved. User has been notified.'), 'status');
    }
    /************************************************
    Check For the Copyright Form is checked or not
    ************************************************/
    if ($form_state['values']['copyrighttransferform'] == 1)
    {
        /* sending email */
        $book_user = user_load($proposal_data->uid);
        $param['proposal_completed']['proposal_id'] = $proposal_id;
        $param['proposal_completed']['user_id'] = $proposal_data->uid;
        $email_to = $book_user->mail;
        if (!drupal_mail('textbook_companion', 'copyrighttransfer_form', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Copyright Form for Book proposal has been recieved. User has been notified.'), 'status');
    } //$form_state['values']['copyrighttransferform'] == 1
    else
    {
        if (!drupal_mail('textbook_companion', 'copyrighttransfer_form_not', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Copyright Transfer Form for Book proposal has not been recieved. User has been notified.'), 'status');
    }
    /************************************************
    Check For the Undertaking Form is checked or not
    ************************************************/
    if ($form_state['values']['undertakingform'] == 1)
    {
        /* sending email */
        $book_user = user_load($proposal_data->uid);
        $param['proposal_completed']['proposal_id'] = $proposal_id;
        $param['proposal_completed']['user_id'] = $proposal_data->uid;
        $email_to = $book_user->mail;
        if (!drupal_mail('textbook_companion', 'undertakingform_form', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Undertaking Form for Book proposal has been recieved. User has been notified.'), 'status');
    } //$form_state['values']['undertakingform'] == 1
    else
    {
        if (!drupal_mail('textbook_companion', 'undertakingform_form_not', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Undertaking Form for Book proposal has not been recieved. User has been notified.'), 'status');
    }
    drupal_set_message(t('Proposal Updated'), 'status');
}

function cheque_status_form($form_state, $proposal_id) {
    global $user;
    /* get current proposal */
    $proposal_id = arg(3);
    /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id =".$proposal_id);*/
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->condition('id', $proposal_id);
    $proposal_q = $query->execute();
    /*$proposal_q1 = db_query("SELECT * FROM {textbook_companion_cheque} WHERE proposal_id =".$proposal_id);*/
    $query = db_select('textbook_companion_cheque');
    $query->fields('textbook_companion_cheque');
    $query->condition('proposal_id', $proposal_id);
    $proposal_q1 = $query->execute();
    $proposal_data1 = $proposal_q1->fetchObject();
    if (!$proposal_data = $proposal_q->fetchObject()) {
        drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
        drupal_goto('manage_proposal');
        return;
    } //!$proposal_data = $proposal_q->fetchObject()
    $form['proposal_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $proposal_id
    );
    /*$empty = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = ".$proposal_id);*/
    $query = db_select('textbook_companion_proposal');
    $query->fields('textbook_companion_proposal');
    $query->condition('id', $proposal_id);
    $empty = $query->execute();
    if (!$empty) {
        /*$prop =db_query("insert into {textbook_companion_cheque} (proposal_id) values(%d)",$proposal_id);*/
        $query = "insert into {textbook_companion_cheque} (proposal_id) values (:proposal_id)";
        $args = array(
            ":proposal_id" => $proposal_id
        );
        $result = db_query($query, $args, array(
            'return' => Database::RETURN_INSERT_ID
        ));
    } //!$empty
    $form['candidate_detail'] = array(
        '#type' => 'fieldset',
        '#title' => t('Candidate Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'candidate_detail'
        )
    );
    $form['candidate_detail']['full_name'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->full_name,
        '#title' => t('Contributor Name')
    );
    $form['candidate_detail']['email'] = array(
        '#type' => 'item',
        '#markup' => user_load($proposal_data->uid)->mail,
        '#title' => t('Email')
    );
    $form['candidate_detail']['mobile'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->mobile,
        '#title' => t('Mobile')
    );
    $form['candidate_detail']['alt_mobile'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data1->alt_mobno,
        '#title' => t('Alternate Mobile No.')
    );
    /*$form_q=db_query("SELECT * FROM {textbook_companion_paper} WHERE proposal_id =".$proposal_id);
    $form_data=db_fetch_object($form_q);*/
    $query = db_select('textbook_companion_paper');
    $query->fields('textbook_companion_paper');
    $query->condition('proposal_id', $proposal_id);
    $result = $query->execute();
    $form_data = $result->fetchObject();
    /* get book preference */
    $preference_html = '<ul>';
    /*$preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d ORDER BY pref_number ASC", $proposal_id);*/
    $query = db_select('textbook_companion_preference');
    $query->fields('textbook_companion_preference');
    $query->condition('proposal_id', $proposal_id);
    $query->orderBy('pref_number', 'ASC');
    $preference_q = $query->execute();
    while ($preference_data = $preference_q->fetchObject())
    {
        if ($preference_data->approval_status == 1)
            $preference_html .= '<li><strong>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')  - Approved Book</strong></li>';
        else
            $preference_html .= '<li>' . $preference_data->book . ' (Written by ' . $preference_data->author . ')</li>';
    } //$preference_data = $preference_q->fetchObject()
    $preference_html .= '</ul>';
    $form['book_preference_f'] = array(
        '#type' => 'fieldset',
        '#title' => t('Book Preferences/Application Status'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'book_preference_f'
        )
    );
    $form['book_preference_f']['book_preference'] = array(
        '#type' => 'item',
        '#markup' => $preference_html,
        '#title' => t('Book Preferences')
    );
    /*$chq_q=db_query("SELECT * FROM {textbook_companion_cheque} WHERE proposal_id = %d", $proposal_id);
    $chq_data=db_fetch_object($chq_q);*/
    $query = db_select('textbook_companion_cheque');
    $query->fields('textbook_companion_cheque');
    $query->condition('proposal_id', $proposal_id);
    $result = $query->execute();
    $chq_data = $result->fetchObject();
    $form_html .= '<ul>';
    if ($form_data->internship_form)
    {
        $form_html .= '<li><strong>Internship Application </strong> Form Submitted</li>';
    } //$form_data->internship_form
    else
    {
        $form_html .= '<li><strong>Internship Application </strong> Form Not Submitted </li>';
    }
    if ($form_data->copyright_form)
    {
        $form_html .= '<li><strong>Copyright Application </strong> Form Submitted</li>';
    } //$form_data->copyright_form
    else
    {
        $form_html .= '<li><strong>Copyright Application</strong> Form Not Submitted </li>';
    }
    if ($form_data->undertaking_form)
    {
        $form_html .= '<li><strong>Undertaking Application </strong> Form Submitted</li>';
    } //$form_data->undertaking_form
    else
    {
        $form_html .= '<li><strong>Undertaking Application</strong> Form Not Submitted </li>';
    }
    $form_html .= '</ul>';
    $form['book_preference_f']['formsubmit'] = array(
        '#type' => 'item',
        '#markup' => $form_html,
        '#title' => t('Application Form Status')
    );
    $form['stu_cheque_details'] = array(
        '#type' => 'fieldset',
        '#title' => t('Student Cheque Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'stu_cheque_details'
        )
    );
    $form['tea_cheque_details'] = array(
        '#type' => 'fieldset',
        '#title' => t('Teacher Cheque Details'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'tea_cheque_details'
        )
    );
    $form['perm_cheque_address'] = array(
        '#type' => 'fieldset',
        '#title' => t('Permanent Address'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'perm_cheque_address'
        )
    );
    $form['temp_cheque_address'] = array(
        '#type' => 'fieldset',
        '#title' => t('Temporary Address'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'temp_cheque_address'
        )
    );
    $form['cheque_delivery'] = array(
        '#type' => 'fieldset',
        '#title' => t('Cheque Delivery'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'cheque_delivery'
        )
    );
    $form['commentf'] = array(
        '#type' => 'fieldset',
        '#title' => t('Remark'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#attributes' => array(
            'id' => 'commentf'
        )
    );
    /*$chq4_q = db_query("SELECT * FROM {textbook_companion_cheque} WHERE proposal_id=".$proposal_id);*/
    $query = db_select('textbook_companion_cheque');
    $query->fields('textbook_companion_cheque');
    $query->condition('proposal_id', $proposal_id);
    $chq4_q = $query->execute();
    $chq1 = '';
    $chq2 = '';
    $chq3 = '';
    $chq4 = '';
    $chq5 = '';
    $chq6 = '';
    $chq7 = '';
    $chq8 = '';
    $chq9 = '';
    $chq10 = '';
    $chq11 = '';
    $chq12 = '';
    $chq13 = '';
    $chq14 = '';
    $chq15 = '';
    $chq16 = '';
    $chq17 = '';
    if ($chqe = $chq4_q->fetchObject())
    {
        $chq1 = $chqe->cheque_no;
        $chq2 = $chqe->address;
        $chq3 = $chqe->cheque_amt;
        $chq4 = $chqe->cheque_sent;
        $chq5 = $chqe->cheque_cleared;
        $chq6 = $chqe->perm_chq_address2;
        $chq7 = $chqe->perm_city;
        $chq8 = $chqe->perm_state;
        $chq9 = $chqe->perm_pincode;
        $chq10 = $chqe->temp_chq_address;
        $chq11 = $chqe->temp_chq_address2;
        $chq12 = $chqe->temp_city;
        $chq13 = $chqe->temp_state;
        $chq14 = $chqe->temp_pincode;
        $chq15 = $chqe->commentf;
        $chq16 = $chqe->t_cheque_amt;
        $chq17 = $chqe->t_cheque_no;
        $form['stu_cheque_details']['cheque_no'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq1,
            '#title' => t('Cheque No'),
            '#size' => 54
        );
        $form['tea_cheque_details']['cheque_no_t'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq17,
            '#title' => t('Cheque No'),
            '#size' => 54
        );
        $form['perm_cheque_address']['chq_address'] = array(
            '#type' => 'textarea',
            '#default_value' => $chq2,
            '#title' => t('Address Street 1')
        );
        $form['perm_cheque_address']['chq_address']['#attributes']['readonly'] = 'readonly';
        $form['perm_cheque_address']['perm_city'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq7,
            '#title' => t('City'),
            '#size' => 35
        );
        $form['perm_cheque_address']['perm_city']['#attributes']['readonly'] = 'readonly';
        $form['perm_cheque_address']['perm_state'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq8,
            '#title' => t('State'),
            '#size' => 35
        );
        $form['perm_cheque_address']['perm_state']['#attributes']['readonly'] = 'readonly';
        $form['perm_cheque_address']['perm_pincode'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq9,
            '#title' => t('Zip code'),
            '#size' => 35
        );
        $form['perm_cheque_address']['perm_pincode']['#attributes']['readonly'] = 'readonly';
        $form['stu_cheque_details']['cheq_amt'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq3,
            '#title' => t('Cheque Amount'),
            '#size' => 54
        );
        $form['tea_cheque_details']['cheq_amt_t'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq17,
            '#title' => t('Cheque Amount'),
            '#size' => 54
        );
        $form['temp_cheque_address']['temp_chq_address'] = array(
            '#type' => 'textarea',
            '#default_value' => $chq10,
            '#title' => t('Address Street 1')
        );
        $form['temp_cheque_address']['temp_chq_address']['#attributes']['readonly'] = 'readonly';
        $form['temp_cheque_address']['temp_city'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq12,
            '#title' => t('City'),
            '#size' => 35
        );
        $form['temp_cheque_address']['temp_city']['#attributes']['readonly'] = 'readonly';
        $form['temp_cheque_address']['temp_state'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq13,
            '#title' => t('State'),
            '#size' => 35
        );
        $form['temp_cheque_address']['temp_state']['#attributes']['readonly'] = 'readonly';
        $form['temp_cheque_address']['temp_pincode'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq14,
            '#title' => t('Zipcode'),
            '#size' => 35
        );
        $form['temp_cheque_address']['temp_pincode']['#attributes']['readonly'] = 'readonly';
    } //$chqe = $chq4_q->fetchObject()
    else
    {
        $form['stu_cheque_details']['cheque_no'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq1,
            '#title' => t('Cheque No')
        );
        $form['tea_cheque_details']['cheque_no_t'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq16,
            '#title' => t('Cheque No')
        );
        $form['perm_cheque_address']['chq_address'] = array(
            '#type' => 'textarea',
            '#default_value' => $chq2,
            '#title' => t('Address Street 1')
        );
        $form['perm_cheque_address']['perm_city'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq7,
            '#title' => t('City'),
            '#size' => 35
        );
        $form['perm_cheque_address']['perm_state'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq8,
            '#title' => t('State'),
            '#size' => 35
        );
        $form['perm_cheque_address']['perm_pincode'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq9,
            '#title' => t('Zip code'),
            '#size' => 35
        );
        $form['perm_cheque_address']['same_address'] = array(
            '#type' => 'checkbox',
            '#title' => t('Same As Permanent Address'),
            '#attributes' => array(
                'onclick' => 'copy_address()'
            )
        );
        $form['stu_cheque_details']['cheq_amt'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq3,
            '#title' => t('Cheque Amount')
        );
        $form['tea_cheque_details']['cheq_amt'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq17,
            '#title' => t('Cheque Amount')
        );
        $form['temp_cheque_address']['temp_chq_address'] = array(
            '#type' => 'textarea',
            '#default_value' => $chq10,
            '#title' => t('Address Street 1')
        );
        $form['temp_cheque_address']['temp_city'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq12,
            '#title' => t('City'),
            '#size' => 35
        );
        $form['temp_cheque_address']['temp_state'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq13,
            '#title' => t('State'),
            '#size' => 35
        );
        $form['temp_cheque_address']['temp_pincode'] = array(
            '#type' => 'textfield',
            '#default_value' => $chq14,
            '#title' => t('Zip code'),
            '#size' => 35
        );
        $form['temp_cheque_address']['same_address'] = array(
            '#type' => 'checkbox',
            '#title' => t('Same As Permanent Address')
        );
        $form['temp_cheque_address']['same_address'] = array(
            '#type' => 'checkbox',
            '#title' => t('Same As Permanent Address'),
            '#attributes' => array(
                'onclick' => 'copy_address()'
            )
        );
    }
    $form['cheque_delivery']['cheque_sent'] = array(
        '#type' => 'checkbox',
        '#title' => t('Cheque Sent'),
        '#default_value' => $chq4,
        '#description' => t('Check if the Cheque has been sent to the user.'),
        '#attributes' => array(
            'id' => 'cheque_sent'
        )
    );
    $form['cheque_delivery']['cheque_cleared'] = array(
        '#type' => 'checkbox',
        '#title' => t('Cheque Cleared'),
        '#default_value' => $chq5,
        '#description' => t('Check if the Cheque has been <strong>Realised</strong> to the User Account.'),
        '#attributes' => array(
            'id' => 'cheque_cleared'
        )
    );
    $form['commentf']['comment_cheque'] = array(
        '#type' => 'textarea',
        '#size' => 35,
        '#attributes' => array(
            'id' => 'comment'
        ),
        '#default_value' => $chq15
    );
    $form['proposal_id'] = array(
        '#type' => 'hidden',
        '#value' => $proposal_id
    );
    /*$preference1_p = db_query("SELECT * FROM {textbook_companion_paper} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id); */
    $query = db_select('textbook_companion_paper');
    $query->fields('textbook_companion_paper');
    $query->condition('proposal_id', $proposal_id);
    $query->orderBy('id', 'ASC');
    $preference1_p = $query->execute();
    if (!($proposal_data1 = $preference1_p->fetchObject()))
    {
        drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
        drupal_goto('manage_proposal');
        return;
    } //!($proposal_data1 = $preference1_p->fetchObject())
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => l(t('Cancel'), 'manage_proposal/all')
    );
    return $form;
}
/*function cheque_status_form_validate($form, &$form_state)
{

//var_dump(($form_state['values']['stu_cheque_details']['cheque_amt']));
//die;
if((!$form_state['values']['cheque_no'])||(!is_numeric($form_state['values']['cheque_no'])))
{
form_set_error('cheque_no', t('Invalid Student Cheque No. Please enter a valid cheque no.'));
return;
}
if((!$form_state['values']['cheq_amt'])||(!is_numeric($form_state['values']['cheq_amt'])))
{
form_set_error('cheq_amt', t('Invalid Student Amount. Please enter a valid numeric amount.'));
return;
}
if((!$form_state['values']['cheque_no_t'])||(!is_numeric($form_state['values']['cheque_no_t'])))
{
form_set_error('cheque_no_t', t('Invalid Teacher Cheque No. Please enter a valid cheque no.'));
return;
}
if((!$form_state['values']['cheq_amt_t'])||(!is_numeric($form_state['values']['cheq_amt_t'])))
{
form_set_error('cheq_amt_t', t('Invalid Teacher Amount. Please enter a valid numeric amount.'));
return;
}
}*/
function cheque_status_form_submit($form, &$form_state)
{
    global $user;
    $proposal_id = arg(2);
    $query = db_update('textbook_companion_cheque');
    $query->fields(array(
        'cheque_no' => $form_state['values']['cheque_no'],
        'cheque_amt' => $form_state['values']['cheq_amt'],
        'alt_mobno' => $form_state['values']['mobileno2'],
        'address' => $form_state['values']['chq_address'],
        'perm_city' => $form_state['values']['perm_city'],
        'perm_state' => $form_state['values']['perm_state'],
        'perm_pincode' => $form_state['values']['perm_pincode'],
        'temp_chq_address' => $form_state['values']['temp_chq_address'],
        'temp_city' => $form_state['values']['temp_city'],
        'temp_state' => $form_state['values']['temp_state'],
        'temp_pincode' => $form_state['values']['temp_pincode'],
        'commentf' => $form_state['values']['comment_cheque'],
        't_cheque_no' => $form_state['values']['cheque_no_t'],
        't_cheque_amt' => $form_state['values']['cheq_amt_t']
    ));
    $query->condition('proposal_id', $proposal_id);
    $num_updated = $query->execute();
    if ($form_state['values']['cheque_sent'] == 1)
    {
        /* sending email */
        /*$query ="UPDATE {textbook_companion_cheque} SET cheque_sent = ".$form_state['values']['cheque_sent']." WHERE proposal_id = ".$proposal_id;
        db_query($query);*/
        $query = db_update('textbook_companion_cheque');
        $query->fields(array(
            'cheque_sent' => $form_state[values][cheque_sent]
        ));
        $query->condition('proposal_id', $proposal_id);
        $num_updated = $query->execute();
        $book_user = user_load($proposal_data->uid);
        $param['proposal_completed']['proposal_id'] = $proposal_id;
        $param['proposal_completed']['user_id'] = $proposal_data->uid;
        $email_to = $book_user->mail;
        if (!drupal_mail('textbook_companion', 'cheque_sent', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Cheque for Book proposal has been Sent. User has been notified.'), 'status');
    } //$form_state['values']['cheque_sent'] == 1
    if ($form_state['values']['cheque_cleared'] == 1)
    {
        /*$query ="UPDATE {textbook_companion_cheque} SET cheque_cleared = ".$form_state['values']['cheque_cleared']." WHERE proposal_id = ".$proposal_id;
        db_query($query);*/
        $query = db_update('textbook_companion_cheque');
        $query->fields(array(
            'cheque_cleared' => $form_state[values][cheque_cleared]
        ));
        $query->condition('proposal_id', $proposal_id);
        $num_updated = $query->execute();
        $curtime = MySQL_NOW();
        echo $curtime;
        drupal_set_message(t('Cheque Has Been Debited into User Account.'), 'status');
        /*$queryc ="UPDATE {textbook_companion_cheque} SET cheque_dispatch_date = NOW() WHERE proposal_id = ".$form_state['values']['proposal_id']."";
        db_query($queryc);*/
        $query = db_update('textbook_companion_cheque');
        $query->fields(array(
            'cheque_dispatch_date' => 'NOW'
        ));
        $query->condition('proposal_id', $form_state['values']['proposal_id']);
        $num_updated = $query->execute();
    } //$form_state['values']['cheque_cleared'] == 1
    /************************************************
    Check For the Remark
    ************************************************/
    if ($form_state['values']['comment_cheque'])
    {
        /* sending email */
        $book_user = user_load($proposal_data->uid);
        $param['proposal_completed']['proposal_id'] = $proposal_id;
        $param['proposal_completed']['user_id'] = $proposal_data->uid;
        $email_to = $book_user->mail;
        if (!drupal_mail('textbook_companion', 'remark', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('Remark Updated. User has been notified'), 'status');
    } //$form_state['values']['comment_cheque']
    else
    {
        if (!drupal_mail('textbook_companion', 'remark_not', $email_to, language_default(), $param, variable_get('textbook_companion_from_email', NULL), TRUE))
            drupal_set_message(t('Error sending email message.'), 'error');
        drupal_set_message(t('No Remarks. User has been notified.'), 'status');
    }
}

function contact_details_form($form, &$form_state) {
  global $user;
  $user_id = $user->uid;
  $query = db_select('textbook_companion_proposal');
  $query->fields('textbook_companion_proposal');
  $query->condition('uid', $user_id);
  $query->orderBy('id', 'DESC');
  $result = $query->execute();
  if(!$proposal_data = $result->fetchObject()) {
    drupal_set_message("Please submit a " . l('proposal', 'proposal') . ".", 'error');
    drupal_goto('');
  }
  $proposal_id = $proposal_data->id;
  $full_name = $proposal_data->full_name;
  $mail = $user->mail;
  $mobno = $proposal_data->mobile;
  $query = db_select('textbook_companion_cheque');
  $query->fields('textbook_companion_cheque');
  $query->condition('proposal_id', $proposal_id);
  $result = $query->execute();
  if(!$cheque_data = $result->fetchObject()) {
    $query = "INSERT INTO {textbook_companion_cheque} (proposal_id) VALUES(:proposal_id)";
    $args = array(
        ":proposal_id" => $proposal_id,
    );
    $result = db_query($query, $args, array(
      'return' => Database::RETURN_INSERT_ID
    ));
  }
  else {
    $perm_address = $cheque_data->address;
    $alt_mobno = $cheque_data->alt_mobno;
    $perm_city = $cheque_data->perm_city;
    $perm_state = $cheque_data->perm_state;
    $perm_pincode = $cheque_data->perm_pincode;
    $temp_address = $cheque_data->temp_chq_address;
    $temp_city = $cheque_data->temp_city;
    $temp_state = $cheque_data->temp_state;
    $temp_pincode = $cheque_data->temp_pincode;
  }
  $form['proposal_id'] = array(
    '#type' => 'hidden',
    '#default_value' => $proposal_id
  );
  $form['candidate_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Candidate Details'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="candidate_details" style="width:50%;">',
    '#suffix' => '</div>',
    '#tree' => TRUE
  );
  $form['candidate_details']['fullname'] = array(
        '#type' => 'textfield',
        '#title' => t('Full Name'),
        '#size' => 48,
        '#default_value' => $full_name
  );
  $form['candidate_details']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#size' => 48,
      '#value' => $mail,
      '#disabled' => TRUE
  );
  $form['candidate_details']['mobileno1'] = array(
      '#type' => 'textfield',
      '#title' => t('Mobile No'),
      '#size' => 48,
      '#default_value' => $mobno
  );
  $form['candidate_details']['mobileno2'] = array(
      '#type' => 'textfield',
      '#title' => t('Alternate Mobile No'),
      '#size' => 48,
      '#default_value' => $alt_mobno
  );
  $query = db_select('textbook_companion_paper_user');
  $query->fields('textbook_companion_paper_user');
  $query->condition('proposal_id', $proposal_id);
  $result = $query->execute();
  $form_data = $result->fetchObject();
  $form_html .= '<ul>';
  if ($form_data->internship_form)
      $form_html .= '<li><strong>Internship Application: </strong> Form Submitted</li>';
  else
      $form_html .= '<li><strong>Internship Application: </strong> Form Not Submitted</li>';
  if ($form_data->copyright_form)
      $form_html .= '<li><strong>Copyright Application: </strong> Form Submitted</li>';
  else
      $form_html .= '<li><strong>Copyright Application: </strong> Form Not Submitted </li>';
  if ($form_data->undertaking_form)
      $form_html .= '<li><strong>Undertaking Application: </strong> Form Submitted</li>';
  else
      $form_html .= '<li><strong>Undertaking Application: </strong> Form Not Submitted </li>';
  $form_html .= '</ul>';
  $form['application_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Application Status'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE ,
    '#value' =>$form_html,
    '#prefix' => '<div id="application_status" style="width:50%;">',
    '#suffix' => '</div>',
    '#tree' => TRUE
  );
  $form['perm_cheque_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Permanent Address'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="perm_address" style="width:50%;">',
    '#suffix' => '</div>',
    '#tree' => TRUE
  );
  $form['perm_cheque_address']['chq_address'] = array(
    '#type' => 'textarea',
    '#title' => t('Address'),
    '#size' => 35,
    '#default_value' => $perm_address
  );
  $form['perm_cheque_address']['perm_city'] = array(
    '#type' => 'textfield',
    '#default_value' => $perm_city,
    '#title' => t('City'),
    '#size' => 35
  );
  $form['perm_cheque_address']['perm_state'] = array(
    '#type' => 'textfield',
    '#default_value' => $perm_state,
    '#title' => t('State'),
    '#size' => 35
  );
  $form['perm_cheque_address']['perm_pincode'] = array(
    '#type' => 'textfield',
    '#default_value' => $perm_pincode,
    '#title' => t('PIN code'),
    '#size' => 35
  );
  $form['temp_cheque_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Temporary Address'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="temp_address" style="width:50%;">',
    '#suffix' => '</div>',
    '#tree' => TRUE
  );
  $form['temp_cheque_address']['temp_chq_address'] = array(
    '#type' => 'textarea',
    '#default_value' => $temp_address,
    '#title' => t('Address'),
    '#size' => 35
  );
  $form['temp_cheque_address']['temp_city'] = array(
    '#type' => 'textfield',
    '#default_value' => $temp_city,
    '#title' => t('City'),
    '#size' => 35
  );
  $form['temp_cheque_address']['temp_state'] = array(
    '#type' => 'textfield',
    '#default_value' => $temp_state,
    '#title' => t('State'),
    '#size' => 35
  );
  $form['temp_cheque_address']['temp_pincode'] = array(
    '#type' => 'textfield',
    '#default_value' => $temp_pincode,
    '#title' => t('PIN code'),
    '#size' => 35
  );
  $form['temp_cheque_address']['same_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Same As Permanent Address'),
    '#attributes' => array(
      'onclick' => 'copy_address()'
    )
  );
  if ($cheque_data->commentf) {
   $form['commentu'] = array(
      '#type' => 'fieldset',
      '#title' => t('Remarks'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#attributes' => array(
          'id' => 'comment_cheque'
      )
    );
    $form['commentu']['comment_cheque'] = array(
      '#type' => 'textarea',
      '#size' => 35,
      '#default_value' => $form16
    );
  } //$chq_data->commentf
  $form['commentu']['comment_cheque']['#attributes']['readonly'] = 'readonly';
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update')
  );
  return $form;
}
function contact_details_form_submit($form, &$form_state) {
  global $user;
  $query = db_update('textbook_companion_cheque');
  $query->fields(array(
    'alt_mobno' => $form_state['values']['candidate_details']['mobileno2'],
    'address' => $form_state['values']['perm_cheque_address']['chq_address'],
    'perm_city' => $form_state['values']['perm_cheque_address']['perm_city'],
    'perm_state' => $form_state['values']['perm_cheque_address']['perm_state'],
    'perm_pincode' => $form_state['values']['perm_cheque_address']['perm_pincode'],
    'temp_chq_address' => $form_state['values']['temp_cheque_address']['temp_chq_address'],
    'temp_city' => $form_state['values']['temp_cheque_address']['temp_city'],
    'temp_state' => $form_state['values']['temp_cheque_address']['temp_state'],
    'temp_pincode' => $form_state['values']['temp_cheque_address']['temp_pincode'],
    'address_con' => 'Submitted'
  ));
  $query->condition('proposal_id', $form_state['values']['proposal_id']);
  $num_updated = $query->execute();
  drupal_set_message(t('Contact Details Has Been Updated'), 'status');
  drupal_goto('accounting/contact_details/user', array(
      'msg' => 0
  ), $fragment = NULL, $http_response_code = 302);
}
function copy_address() {
  $form['temp_cheque_address']['temp_chq_address'] = array(
    '#type' => 'textarea',
    '#default_value' => $form['values']['perm_cheque_address']['chq_address'],
    '#title' => t('Address Street 1')
  );
  $form['temp_cheque_address']['temp_city'] = array(
    '#type' => 'textfield',
    '#default_value' => $form['values']['perm_cheque_address']['perm_city'],
    '#title' => t('City'),
    '#size' => 35
  );
  $form['temp_cheque_address']['temp_state'] = array(
    '#type' => 'textfield',
    '#default_value' => $form['values']['perm_cheque_address']['perm_state'],
    '#title' => t('State'),
    '#size' => 35
  );
  $form['temp_cheque_address']['temp_pincode'] = array(
    '#type' => 'textfield',
    '#default_value' => $form['values']['perm_cheque_address']['perm_pincode'],
    '#title' => t('Zip code'),
    '#size' => 35
  );
}
function tracking_details_ajax($form, &$form_state) {
  return $form['tracking_details'];
}
