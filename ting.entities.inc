<?php

/**
 * @file
 * Entity classes for ting objects and collections.
 */

/**
 * Object representing a relation between TingEntites.
 */
class TingRelation extends DingEntityBase {

  public $type = DingEntityBase::NULL;

  public $uri = DingEntityBase::NULL;

  public $object = DingEntityBase::NULL;

  public function __construct($type, $uri, $object = NULL) {
    parent::__construct();
    $this->properties['type'] = $type;
    $this->properties['uri'] = $uri;
    if ($object) {
      $this->properties['object'] = $object;
    }
  }

  public function getObject() {
    return $this->properties['object'];
  }

}

/**
 * Ting object entity.
 */
class TingEntity extends DingEntity {

  public $type = DingEntityBase::NULL;

  public $serieNumber = DingEntityBase::NULL;

  public $serieTitle = DingEntityBase::NULL;

  public $record = DingEntityBase::NULL;

  public $relations = DingEntityBase::NULL;

  public $localId = DingEntityBase::NULL;

  public $ownerId = DingEntityBase::NULL;

  public $creators = DingEntityBase::NULL;

  public $subjects = DingEntityBase::NULL;

  public $language = DingEntityBase::NULL;

  public $date = DingEntityBase::NULL;

  public $onlineUrl = DingEntityBase::NULL;

  public $source = DingEntityBase::NULL;

  public $description = DingEntityBase::NULL;

  public $contributors = DingEntityBase::NULL;

  public $isPartOf = DingEntityBase::NULL;

  public $extent = DingEntityBase::NULL;

  public $classification = DingEntityBase::NULL;

  public $isbn = DingEntityBase::NULL;

  public $descriptionSeries = DingEntityBase::NULL;

  public function getExtent() {
    $extent = $this->data->get('extent');
    return !empty($extent) ? $extent : FALSE;
  }

  function getClassification() {
    $dk5 = $this->data->get('subject_dk5');
    if ($dk5 == 'sk') {
      $dk5 = '';
    }
    return $dk5;
  }

  function getIsPartOf() {
    $isPartOf = $this->data->get('ispartof');
    return $isPartOf;
  }

  // pjo 26-09-11 ; getmethod contributor
  function getContributors() {
    $contributors = $this->data->get('contributor');
    return $contributors;
  }

  function getLocalId() {
    $id = $this->data->getId();
    return !empty($id) ? $id : FALSE;
  }

  function getOwnerId() {
    $owner = $this->data->get('ownerid');
    return !empty($owner) ? $owner : FALSE;
  }

  function getType() {
    $type = $this->data->get('type_bibdktype');
    return !empty($type) ? $type : FALSE;
  }

  function getTitle() {
    $title = FALSE;
    $title1 = $this->data->get('title');
    $title2 = $this->data->get('title_full');

    if (!empty($title2)) {
      $title = $title2;
    }
    elseif (!empty($title1)) {
      $title = $title1;
    }
    return $title;
  }

  function getSerieNumber() {
    $serie = $this->splitSerie();
    // Last part is the number.
    if (empty($serie[1])) {
      return FALSE;
    }
    return $serie[1];
  }

  private function splitSerie() {
    $serie = $this->data->get('title_series');
    return explode(';', $serie);
  }

  function getSerieTitle() {
    $serie = $this->splitSerie();
    // First part is the title.
    if (empty($serie[0])) {
      return FALSE;
    }
    return trim($serie[0]);
  }

  function getAbstract() {
    $abstract = $this->data->get('abstract');
    return !empty($abstract) ? $abstract : FALSE;
  }

  function getRelations() {
    $relations = $this->data->getRelations();

    $this->relations = array();
    $relation_objects = array();
    $entity_ids = array();

    foreach ($relations as $relation) {
      if (!empty($relation['object'])) {
        $entity_ids[] = $relation['uri'];
      }
    }
    if (!empty($entity_ids)) {
      $objects = entity_load('ting_object', array(), array('ding_entity_id' => $entity_ids));
      foreach ($objects as $object) {
        $relation_objects[$object->id] = $object;
      }
    }

    foreach ($relations as $relation) {
      $this->relations[] = new TingRelation($relation['type'], $relation['uri'], isset($relation_objects[$relation['uri']]) ? $relation_objects[$relation['uri']] : NULL);
    }

    return $this->relations;
  }

  function getCreators() {
    $creators = $this->data->get('creator');
    if (!is_array($creators)) {
      $creators = array($creators);
    }
    return $creators;
  }

  function getSubjects() {
    $subjects = $this->data->get('subject');
    if (!empty($subjects) && !is_array($subjects)) {
      $subjects = array($subjects);
    }
    return $subjects;
  }

  function getLanguage() {
    return !empty($this->reply->record['dc:language'][''][0]) ? $this->reply->record['dc:language'][''][0] : FALSE;
  }

  function getDate() {
    $date = $this->data->get('date');
    return !empty($date) ? $date : FALSE;
  }

  function getOnlineUrl() {
    $url = $this->data->get('identifier_uri');
    if (!empty($url)) {
      drupal_alter('ting_online_url', $url, $this);
      return $url;
    }
  }

  function getSource() {
    return !empty($this->reply->record['ac:source'][''][0]) ? $this->reply->record['ac:source'][''][0] : FALSE;
  }

  function getDescription() {
    return !empty($this->reply->record['dc:description'][''][0]) ? $this->reply->record['dc:description'][''][0] : FALSE;
  }

  /**
   * Get ISBN numbers of the object.
   *
   * @return array
   */
  function getIsbn() {
    $isbn = $this->data->get('identifier_isbn');

    // Nothing to do.
    if (empty($isbn)) {
      return array();
    }

    if (!is_array($isbn)) {
      $isbn = array($isbn);
    }

    // Get ISBN numbers.
    foreach ($isbn as $k => $number) {
      $isbn[$k] = str_replace(array(' ','-'), '', $number);
    }
    rsort($isbn);
    return $isbn;
  }

  public function getDescriptionSeries() {
    return $this->data->get('description_series');
  }

  public function get($key) {
    return $this->data->get($key);
  }
}

