<?php
class TingSearchResult implements Iterator, Countable {
  /**
   * Raw data from webservice
   * @var OutputInterface
   */
  private $result;

  /**
   * Number of objects found.
   * @var int
   */
  protected $hitCount;

  /**
   * Objects on this page.
   * @var int
   */
  protected $collectionCount;

  /**
   * Availability of other pages.
   * @var bool
   */
  protected $hasMore;

  /**
   * Items in current result set.
   * Contains TingObject or TingEntity.
   * @var array
   */
  protected $items = array();

  // Data for UI.
  protected $keyword;
  protected $perPage = 0;

  protected $facets = array();

  // Iterator
  protected $_position;
  public function __construct(OutputInterface $result, TingSearchRequest $request) {
    $this->_position = 0;
    $this->perPage = $request->getParameter('stepValue');

    $this->result = $result;
    $this->process();
  }

  /**
   * Build items from raw data (json).
   */
  protected function process() {
    // Check for errors.
    $error = $this->result->getValue('searchResponse/error');
    if (!empty($error)) {
      throw new TingClientException($error);
    }

    $data = $this->result->getValue('searchResponse/result');

    $this->hitCount = $data->getValue('hitCount');
    $this->collectionCount = $data->getValue('collectionCount');
    $this->hasMore = filter_var($data->getValue('more'), FILTER_VALIDATE_BOOLEAN);

    $items = $data->getValue('searchResult', 1);
    if (!empty($items)) {
      foreach ($items as $item) {
        $this->items[] = new TingObject($item);
      }
    }

    $facets = $data->getValue('facetResult/facet');
    if (!empty($facets)) {
      foreach ($facets as $facet) {
        $facetObject = new TingClientFacetResult();
        $facetObject->name = $facet->getValue('facetName');
        $terms = $facet->getValue('facetTerm');

        if (!empty($terms)) {
          foreach ($terms as $term) {
            $value = $term->getValue('frequence');
            $key = $term->getValue('term');
            $facetObject->terms[$key] = $value;
          }
        }

        $this->facets[$facetObject->name] = $facetObject;
      }
   }

    $this->result = null;
  }

  public function setKeyword($keys) {
    $this->keyword = $keys;
  }

  public function getKeyword() {
    return $this->keyword;
  }

  public function hasMore() {
    return $this->hasMore;
  }

  public function getTotal() {
    return $this->hitCount;
  }

  public function setItems($items) {
    $this->items = array();
    foreach ($items as $item) {
      $this->items[] = $item;
    }
    $this->rewind();
  }

  public function getFacets() {
    return $this->facets;
  }

  public function getPerPage() {
    return $this->perPage;
  }

  /*
   * (non-PHPdoc) @see Iterator::current()
   */
  public function current() {
    return $this->items[$this->_position];
  }

  /*
   * (non-PHPdoc) @see Iterator::next()
   */
  public function next() {
    ++$this->_position;
  }

  /*
   * (non-PHPdoc) @see Iterator::key()
   */
  public function key() {
    return $this->_position;
  }

  /*
   * (non-PHPdoc) @see Iterator::valid()
   */
  public function valid() {
    return isset($this->items[$this->_position]);
  }

  /*
   * (non-PHPdoc) @see Iterator::rewind()
   */
  public function rewind() {
    $this->_position = 0;
  }

  /*
   * (non-PHPdoc) @see Countable::count()
   */
  public function count() {
    return $this->collectionCount;
  }
}
