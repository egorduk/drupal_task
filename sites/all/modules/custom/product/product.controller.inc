<?php
class ProductController extends EntityAPIController {
    public function create(array $values = array()) {
        global $product, $image, $type;
        dpm($product);
        $values += array(
            //'id' => $product->id,
            'name' => $product->name,
            'info' => $product->info,
            'created_at' => REQUEST_TIME,
            'updated_at' => REQUEST_TIME,
            'image_id' => 1,
            'type_id' => 1,
        );
        return parent::create($values);
    }

    public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
        $wrapper = entity_metadata_wrapper('product', $entity);
        //$content['author'] = array('#markup' => t('Created by: !author', array('!author' => $wrapper->uid->name->value(array('sanitize' => TRUE)))));
        // Make Description and Status themed like default fields.
        $content['description'] = array(
            '#theme' => 'field',
            '#weight' => 0,
            '#title' =>t('Description'),
            '#access' => TRUE,
            '#label_display' => 'above',
            '#view_mode' => 'full',
            '#language' => LANGUAGE_NONE,
            '#field_name' => 'field_fake_description',
            '#field_type' => 'text',
            '#entity_type' => 'product',
            //'#bundle' => $entity->type,
            '#items' => array(array('value' => $entity->info)),
            '#formatter' => 'text_default',
            0 => array('#markup' => check_plain($entity->info))
        );
        return parent::buildContent($entity, $view_mode, $langcode, $content);
    }

    public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
        // Основную работу делают за нас.
        $view = parent::view($entities, $view_mode, $langcode, $page);
        if (!empty($view)) {
            // Мы просто поставим свою темизацию, соберём id сообщений,
            // у которых надо обновить время прочтения и, попутно, покажем
            // для таких сообщений, что они прочитаны, прямо в текущем выводе.
            global $user;
            $readed_ids = array();
            $read_time = REQUEST_TIME;
                foreach ($view[$this->entityType] as &$entity) {
                $smessage = &$entity['#entity'];
                if ($smessage->recipient_uid == $user->uid && 'full' == $view_mode) {
                    $readed_ids[] = $smessage->smessage_id;
                    // Показываем, что сообщение прочитано, для текущего вывода.
                    $smessage->read_time = $read_time;
                }
                // Наша темизация.
                $entity['#theme'] = $this->entityType;
            }
            unset($entity);
            // Сохраняем для выбранных сообщений время прочтения.
            if ($readed_ids) {
                db_update('product')
                    ->fields(array('read_time' => $read_time))
                    ->condition('id', $readed_ids)
                    ->execute();
            }
        }
        return $view;
    }
}