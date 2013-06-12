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

  protected $type = DingEntityBase::NULL;

  protected $serieNumber = DingEntityBase::NULL;

  protected $serieTitle = DingEntityBase::NULL;

  protected $record = DingEntityBase::NULL;

  protected $relations = DingEntityBase::NULL;

  protected $localId = DingEntityBase::NULL;

  protected $ownerId = DingEntityBase::NULL;

  protected $creators = DingEntityBase::NULL;

  protected $date = DingEntityBase::NULL;

  protected $subjects = DingEntityBase::NULL;

  protected $language = DingEntityBase::NULL;

  protected $onlineUrl = DingEntityBase::NULL;

  protected $acSource = DingEntityBase::NULL;

  protected $description = DingEntityBase::NULL;

  protected $contributors = DingEntityBase::NULL;

  protected $isPartOf = DingEntityBase::NULL;

  protected $extent = DingEntityBase::NULL;

  protected $classification = DingEntityBase::NULL;

  protected $isbn = DingEntityBase::NULL;

  protected $descriptionSeries = DingEntityBase::NULL;

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

  function getSubjectDK5Text() {
    $result = array();
    $dk5 = $this->data->get('subject_dk5text');
    if (!empty($dk5)) {
      if (!is_array($dk5)) {
        $dk5 = array($dk5);
      }
      $result = $dk5;
    }
    return $result;
  }

  function getSubjectDBCS() {
    $result = array();
    $subjects = $this->data->get('subject_dbcs');
    if (!empty($subjects)) {
      if (!is_array($subjects)) {
        $subjects = array($subjects);
      }
      $result = $subjects;
    }
    return $result;
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
    $id = explode(':', $this->data->getObjectId());
    return $id[1];
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
    $creators = array();

    // Get dc:creator.
    $_creators = $this->data->get('creator');
    if (!empty($_creators) && !is_array($_creators)) {
      $_creators = array($_creators);
    }

    // Get dc:creator oss:aut.
    if (empty($_creators)) {
      $_creators = $this->data->get('creator_aut');

      if (!empty($_creators) && !is_array($_creators)) {
        $_creators = array($_creators);
      }
    }

    return $creators;
  }

  public function getCreatorMus() {
    return $this->data->get('creator_mus');
  }

  function getSubjects() {
    $subjects = $this->data->get('subject');
    if (!empty($subjects) && !is_array($subjects)) {
      $subjects = array($subjects);
    }
    return $subjects;
  }

  function getLanguage() {
    $language = $this->data->get('language');
    return !empty($language) ? $language : FALSE;
  }

  function getLanguageIso() {
    $language = $this->data->get('language_iso639_2');
    return !empty($language) ? $language : FALSE;
  }

  function getDate() {
    $date = $this->data->get('date');
    return !empty($date) ? $date : FALSE;
  }

  function getSubjectGenre() {
    $date = $this->data->get('subject_genre');
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
    $source = $this->data->get('source');
    return !empty($source) ? $source : FALSE;
  }

  function getAcSource() {
    $source = $this->data->get('ac_source');
    return !empty($source) ? $source : FALSE;
  }

  function getDescription() {
    $description = $this->data->get('description');
    return !empty($description) ? $description : FALSE;
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
      $isbn[$k] = str_replace(array(' ', '-'), '', $number);
    }
    rsort($isbn);
    return $isbn;
  }

  function getIssn() {
    return $this->data->get('identifier_issn');
  }

  public function getDescriptionSeries() {
    return $this->data->get('description_series');
  }

  public function getFormat() {
    return $this->data->get('format');
  }

  public function getLanguageSpoken() {
    return $this->data->get('language_spoken');
  }

  public function getLanguageSubtitles() {
    return $this->data->get('language_subtitles');
  }

  public function getSpatial() {
    return $this->data->get('spatial');
  }

  public function getHaspartTrack() {
    return $this->data->get('haspart_track');
  }

  public function getIsreferencedby() {
    return $this->data->get('isreferencedby');
  }

  public function getVersion() {
    return $this->data->get('version');
  }

  public function getPublisher() {
    return $this->data->get('publisher');
  }

  public function getRights() {
    return $this->data->get('rights');
  }

  public function getAudience() {
    return $this->data->get('audience');
  }

  public function getAudienceAge() {
    return $this->data->get('audience_age');
  }

  public function getAudiencePegi() {
    return $this->data->get('audience_pegi');
  }

  public function getAudienceMedieraad() {
    return $this->data->get('audience_medieraad');
  }

  public function get($key) {
    return $this->data->get($key);
  }

  public function __get($prop) {
    if (method_exists($this, 'get' . ucfirst($prop))) {
      kpr(ddebug_backtrace());
      throw new Exception('Use getter methor for: ' . $prop);
    }
    return parent::__get($prop);
  }

  public function getFormatsAvailable() {
    return $this->data->getFormats();
  }

  public function getIdentifier() {
    return $this->data->get('identifier');
  }

  public function getIdentifierUri() {
    return $this->data->get('identifier_uri');
  }

  /**
   * Get first non-empty value of the fields.
   * Accepts any number of field names as parameters.
   *
   * @return NULL | mixed
   */
  public function getOneOf() {
    $fields = func_get_args();
    $result = NULL;
    foreach ($fields as $field) {
      $value = $this->get($field);
      if (!empty($value)) {
        $result = $value;
        break;
      }
    }
    return $result;
  }

}
